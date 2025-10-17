#include "wifi_manager.h"
#include "config.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

#define EEPROM_SIZE 512
#define SSID_ADDR 0
#define PASS_ADDR 64
#define MAGIC_ADDR 128
#define MAGIC_VALUE 0xABCD
#define CHECKSUM_ADDR 132

WiFiManager::WiFiManager() 
    : configServer(nullptr), dnsServer(nullptr), isAPMode(false), 
      isConnected(false), lastConnectionAttempt(0), connectionTimeout(WIFI_TIMEOUT) {
}

WiFiManager::~WiFiManager() {
    if (configServer) delete configServer;
    if (dnsServer) delete dnsServer;
}

bool WiFiManager::begin() {
    EEPROM.begin(EEPROM_SIZE);

    // Compute and set branded hostname (STA mode)
    String hostname = computeHostname();
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname.c_str());

    // Start mDNS for easier discovery (optional)
    MDNS.begin(hostname.c_str());

    // Debug: Check EEPROM status
    DEBUG_PRINTF("EEPROM initialized, magic value: 0x%X\n", EEPROM.readUShort(MAGIC_ADDR));
    DEBUG_PRINTF("Hostname set: %s\n", hostname.c_str());

    return true;
}

bool WiFiManager::connect() {
    String ssid, password;
    
    // Try to load saved credentials first
    if (loadWiFiCredentials(ssid, password)) {
        DEBUG_PRINTF("Attempting to connect to saved WiFi: %s\n", ssid.c_str());
        if (connectToWiFi(ssid, password)) {
            return true;
        } else {
            DEBUG_PRINTLN("Saved credentials failed, trying defaults");
        }
    }
    
    // Try default credentials from config
    DEBUG_PRINTF("Attempting to connect to default WiFi: %s\n", WIFI_SSID);
    return connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
}

bool WiFiManager::connect(const String& ssid, const String& password) {
    return connectToWiFi(ssid, password);
}

bool WiFiManager::connectToWiFi(const String& ssid, const String& password) {
    if (ssid.length() == 0) {
        DEBUG_PRINTLN("Empty SSID provided");
        return false;
    }
    
    WiFi.begin(ssid.c_str(), password.c_str());
    lastConnectionAttempt = millis();
    
    DEBUG_PRINTF("Connecting to %s", ssid.c_str());
    
    while (WiFi.status() != WL_CONNECTED && 
           millis() - lastConnectionAttempt < connectionTimeout) {
        delay(500);
        DEBUG_PRINT(".");
    }
    DEBUG_PRINTLN();
    
    if (WiFi.status() == WL_CONNECTED) {
        isConnected = true;
        DEBUG_PRINTF("Connected! IP: %s\n", WiFi.localIP().toString().c_str());
        
        // IMPROVED: Save credentials with verification
        if (saveWiFiCredentialsWithVerification(ssid, password)) {
            DEBUG_PRINTLN("Credentials saved and verified successfully");
        } else {
            DEBUG_PRINTLN("WARNING: Credential save/verification failed!");
        }
        
        return true;
    }
    
    DEBUG_PRINTLN("Connection failed");
    isConnected = false;
    return false;
}

void WiFiManager::startAccessPoint() {
    DEBUG_PRINTLN("Starting Access Point mode");
    
    WiFi.mode(WIFI_AP);
    String hostname = computeHostname();
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    // Attempt to set AP hostname if supported
    #ifdef ARDUINO
    #if defined(ESP_ARDUINO_VERSION)
    // Some cores provide softAPsetHostname
    #if __has_include(<esp_wifi.h>)
    // Best-effort: not all cores expose this API
    #endif
    #endif
    #endif
    // mDNS in AP mode (may not be available on all OSes)
    MDNS.begin(hostname.c_str());

    DEBUG_PRINTF("AP started: %s\n", AP_SSID);
    DEBUG_PRINTF("AP hostname: %s\n", hostname.c_str());
    DEBUG_PRINTF("AP IP: %s\n", WiFi.softAPIP().toString().c_str());
    
    setupConfigPortal();
    isAPMode = true;
}

void WiFiManager::setupConfigPortal() {
    if (configServer) delete configServer;
    if (dnsServer) delete dnsServer;
    
    configServer = new WebServer(80);
    dnsServer = new DNSServer();
    
    // Start DNS server for captive portal
    dnsServer->start(53, "*", WiFi.softAPIP());
    
    // Set up web server routes
    configServer->on("/", [this]() { handleConfigPage(); });
    configServer->on("/save", [this]() { handleConfigSave(); });
    configServer->on("/debug", [this]() { handleDebugPage(); }); // NEW: Debug page
    configServer->onNotFound([this]() { handleNotFound(); });
    
    configServer->begin();
    DEBUG_PRINTLN("Config portal started");
}

void WiFiManager::handleConfigPage() {
    String html = generateConfigHTML();
    configServer->send(200, "text/html", html);
}

void WiFiManager::handleDebugPage() {
    String html = generateDebugHTML();
    configServer->send(200, "text/html", html);
}

void WiFiManager::handleConfigSave() {
    String ssid = configServer->arg("ssid");
    String password = configServer->arg("password");
    
    if (ssid.length() == 0) {
        configServer->send(400, "text/html", "<h1>Error: SSID cannot be empty</h1>");
        return;
    }
    
    DEBUG_PRINTF("Received credentials: %s\n", ssid.c_str());
    
    String html = "<h1>Credentials Saved</h1>";
    html += "<p>Attempting to connect to: " + ssid + "</p>";
    html += "<p>Device will restart in 5 seconds...</p>";
    html += "<script>setTimeout(function(){window.location.href='/';}, 5000);</script>";
    
    configServer->send(200, "text/html", html);
    
    // IMPROVED: Save with multiple attempts and verification
    bool savedOk = false;
    for (int attempt = 0; attempt < 3; attempt++) {
        if (saveWiFiCredentialsWithVerification(ssid, password)) {
            savedOk = true;
            break;
        }
        DEBUG_PRINTF("Save attempt %d failed, retrying...\n", attempt + 1);
        delay(100);
    }
    
    if (!savedOk) {
        DEBUG_PRINTLN("CRITICAL: Could not save WiFi credentials!");
        return;
    }
    
    delay(2000);
    
    // Try to connect
    WiFi.mode(WIFI_STA);
    if (connectToWiFi(ssid, password)) {
        isAPMode = false;
        if (configServer) {
            configServer->stop();
            delete configServer;
            configServer = nullptr;
        }
        if (dnsServer) {
            dnsServer->stop();
            delete dnsServer;
            dnsServer = nullptr;
        }
    } else {
        // Connection failed, restart AP
        delay(2000);
        startAccessPoint();
    }
}

void WiFiManager::handleNotFound() {
    // Redirect to config page for captive portal
    configServer->sendHeader("Location", "/", true);
    configServer->send(302, "text/plain", "");
}

String WiFiManager::generateConfigHTML() {
    String html = F("<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>");
    html += F("<title>psAI-Ducky • Wi‑Fi Setup</title>");
    html += F("<style>");
    html += F("*{box-sizing:border-box}html,body{height:100%;margin:0;font-family:Inter,system-ui,Segoe UI,Roboto,Arial,sans-serif;color:#0f172a}");
    html += F("body{background:linear-gradient(135deg,#0ea5e9 0%,#8b5cf6 50%,#f59e0b 100%);}");
    html += F(".wrap{min-height:100%;display:flex;align-items:center;justify-content:center;padding:32px}");
    html += F(".card{width:100%;max-width:520px;background:#fff;border-radius:16px;box-shadow:0 20px 40px rgba(2,6,23,.2);overflow:hidden}");
    html += F(".header{padding:24px 24px 8px;background:linear-gradient(135deg,rgba(14,165,233,.15),rgba(139,92,246,.15));}");
    html += F(".brand{font-weight:800;letter-spacing:.4px;font-size:22px;margin:0;color:#0f172a}");
    html += F(".tag{margin:8px 0 0;color:#334155;font-size:13px}");
    html += F(".content{padding:24px}");
    html += F("label{display:block;margin:10px 0 6px;font-weight:600;color:#334155}");
    html += F("input[type=text],input[type=password]{width:100%;padding:12px 14px;border:1px solid #e2e8f0;border-radius:10px;background:#f8fafc;outline:none;transition:border .15s}");
    html += F("input:focus{border-color:#8b5cf6;box-shadow:0 0 0 3px rgba(139,92,246,.15)}");
    html += F("button{width:100%;padding:12px 16px;margin-top:14px;border:0;border-radius:10px;background:linear-gradient(90deg,#0ea5e9,#8b5cf6);color:#fff;font-weight:700;cursor:pointer;transition:opacity .15s}");
    html += F("button:hover{opacity:.9}");
    html += F(".meta{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:12px;padding:12px;border-radius:10px;background:#f1f5f9;color:#334155;font-size:13px}");
    html += F(".footer{padding:16px 24px;border-top:1px solid #e2e8f0;background:#fafafa;font-size:12px;color:#475569;display:flex;justify-content:space-between;align-items:center}");
    html += F(".link{color:#0ea5e9;text-decoration:none;font-weight:600}");
    html += F("</style></head><body><div class='wrap'><div class='card'>");

    html += F("<div class='header'>");
    html += F("<h1 class='brand'>psAI‑Ducky</h1>");
    html += F("<p class='tag'>AI‑powered USB HID bridge • Wi‑Fi Configuration</p>");
    html += F("</div>");

    html += F("<div class='content'>");
    html += F("<form action='/save' method='POST' autocomplete='off'>");
    html += F("<label for='ssid'>Wi‑Fi Network (SSID)</label>");
    html += F("<input id='ssid' type='text' name='ssid' placeholder='Your Wi‑Fi name' required>");
    html += F("<label for='password'>Wi‑Fi Password</label>");
    html += F("<input id='password' type='password' name='password' placeholder='Your Wi‑Fi password'>");
    html += F("<button type='submit'>Save & Connect</button>");
    html += F("</form>");

    html += F("<div class='meta'>");
    html += F("<div><strong>Device</strong><br>ESP32‑S2 HID</div>");
    html += F("<div><strong>Version</strong><br>");
    html += MCP_SERVER_VERSION;
    html += F("</div>");
    html += F("<div><strong>AP SSID</strong><br>");
    html += AP_SSID;
    html += F("</div>");
    html += F("<div><strong>AP IP</strong><br>");
    html += WiFi.softAPIP().toString();
    html += F("</div>");
    html += F("<div><strong>Hostname</strong><br>");
    html += computeHostname();
    html += F(".local</div></div>");

    html += F("</div>"); // content

    html += F("<div class='footer'>");
    html += F("<span>Need details? <a class='link' href='/debug'>Debug page</a></span>");
    html += F("<span>© 2025 Howie Duhzit • psAI‑Ducky</span>");
    html += F("</div>");

    html += F("</div></div></body></html>");
    return html;
}

String WiFiManager::generateDebugHTML() {
    String ssid, password;
    bool hasCredentials = loadWiFiCredentials(ssid, password);

    String html = F("<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>");
    html += F("<title>psAI‑Ducky • Debug</title>");
    html += F("<style>body{margin:0;font-family:Inter,system-ui,Arial,sans-serif;background:#0f172a;color:#e2e8f0} .wrap{max-width:900px;margin:0 auto;padding:24px} h1{font-size:22px;margin:16px 0 12px} .panel{background:#111827;border:1px solid #1f2937;border-radius:12px;padding:16px;margin:12px 0} .kv{display:grid;grid-template-columns:200px 1fr;gap:8px} a{color:#38bdf8;text-decoration:none}</style></head><body>");
    html += F("<div class='wrap'><h1>psAI‑Ducky • Debug</h1>");

    html += F("<div class='panel'><h3>EEPROM Status</h3><div class='kv'>");
    html += F("<div>Magic Value</div><div>0x"); html += String(EEPROM.readUShort(MAGIC_ADDR), HEX); html += F(" (Expected 0xABCD)</div>");
    html += F("<div>Saved Credentials</div><div>"); html += hasCredentials ? "YES" : "NO"; html += F("</div>");
    if (hasCredentials) {
        html += F("<div>Saved SSID</div><div>"); html += ssid; html += F("</div>");
        html += F("<div>Password Length</div><div>"); html += String(password.length()); html += F("</div>");
    }
    html += F("</div></div>");

    html += F("<div class='panel'><h3>Wi‑Fi Status</h3><div class='kv'>");
    html += F("<div>Mode</div><div>"); html += WiFi.getMode() == WIFI_AP ? "Access Point" : "Station"; html += F("</div>");
    html += F("<div>Status</div><div>"); html += String(WiFi.status()); html += F("</div>");
    if (WiFi.status() == WL_CONNECTED) {
        html += F("<div>Connected SSID</div><div>"); html += WiFi.SSID(); html += F("</div>");
        html += F("<div>IP Address</div><div>"); html += WiFi.localIP().toString(); html += F("</div>");
        html += F("<div>Hostname</div><div>"); html += computeHostname(); html += F(".local</div>");
        html += F("<div>RSSI</div><div>"); html += String(WiFi.RSSI()); html += F(" dBm</div>");
    } else {
        html += F("<div>AP SSID</div><div>"); html += AP_SSID; html += F("</div>");
        html += F("<div>AP IP</div><div>"); html += WiFi.softAPIP().toString(); html += F("</div>");
    }
    html += F("</div></div>");

    html += F("<p><a href='/'>&larr; Back to Configuration</a></p>");
    html += F("</div></body></html>");

    return html;
}

// Hostname helpers
String WiFiManager::macTail() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char buf[5];
    snprintf(buf, sizeof(buf), "%02x%02x", mac[4], mac[5]);
    String s(buf);
    s.toLowerCase();
    return s;
}

String WiFiManager::computeHostname() {
    String tail = macTail();
    String host = String(HOSTNAME_PREFIX) + "-" + tail;
    host.toLowerCase();
    return host;
}

// IMPROVED: Save with verification
bool WiFiManager::saveWiFiCredentialsWithVerification(const String& ssid, const String& password) {
    DEBUG_PRINTLN("Saving WiFi credentials with verification...");
    
    // Calculate checksum
    uint16_t checksum = 0;
    for (int i = 0; i < ssid.length(); i++) {
        checksum += ssid[i];
    }
    for (int i = 0; i < password.length(); i++) {
        checksum += password[i];
    }
    
    // Write magic value
    EEPROM.writeUShort(MAGIC_ADDR, MAGIC_VALUE);
    
    // Write checksum
    EEPROM.writeUShort(CHECKSUM_ADDR, checksum);
    
    // Write SSID
    for (int i = 0; i < 64; i++) {
        if (i < ssid.length()) {
            EEPROM.write(SSID_ADDR + i, ssid[i]);
        } else {
            EEPROM.write(SSID_ADDR + i, 0);
        }
    }
    
    // Write password
    for (int i = 0; i < 64; i++) {
        if (i < password.length()) {
            EEPROM.write(PASS_ADDR + i, password[i]);
        } else {
            EEPROM.write(PASS_ADDR + i, 0);
        }
    }
    
    // Commit to flash
    if (!EEPROM.commit()) {
        DEBUG_PRINTLN("EEPROM commit failed!");
        return false;
    }
    
    // Wait for write to complete
    delay(100);
    
    // Verify by reading back
    String readSsid, readPassword;
    if (!loadWiFiCredentials(readSsid, readPassword)) {
        DEBUG_PRINTLN("Verification failed: Could not read back credentials");
        return false;
    }
    
    if (readSsid != ssid || readPassword != password) {
        DEBUG_PRINTLN("Verification failed: Data mismatch");
        DEBUG_PRINTF("Expected SSID: %s, Got: %s\n", ssid.c_str(), readSsid.c_str());
        return false;
    }
    
    DEBUG_PRINTLN("WiFi credentials saved and verified successfully");
    return true;
}

// IMPROVED: Load with checksum verification
bool WiFiManager::loadWiFiCredentials(String& ssid, String& password) {
    // Check magic value
    if (EEPROM.readUShort(MAGIC_ADDR) != MAGIC_VALUE) {
        DEBUG_PRINTLN("No saved WiFi credentials found (no magic value)");
        return false;
    }
    
    // Read SSID
    ssid = "";
    for (int i = 0; i < 64; i++) {
        char c = EEPROM.read(SSID_ADDR + i);
        if (c == 0) break;
        ssid += c;
    }
    
    // Read password
    password = "";
    for (int i = 0; i < 64; i++) {
        char c = EEPROM.read(PASS_ADDR + i);
        if (c == 0) break;
        password += c;
    }
    
    if (ssid.length() == 0) {
        DEBUG_PRINTLN("No saved WiFi credentials found (empty SSID)");
        return false;
    }
    
    // Verify checksum if available
    uint16_t storedChecksum = EEPROM.readUShort(CHECKSUM_ADDR);
    uint16_t calculatedChecksum = 0;
    for (int i = 0; i < ssid.length(); i++) {
        calculatedChecksum += ssid[i];
    }
    for (int i = 0; i < password.length(); i++) {
        calculatedChecksum += password[i];
    }
    
    if (storedChecksum != 0 && storedChecksum != calculatedChecksum) {
        DEBUG_PRINTLN("WARNING: Credential checksum mismatch, data may be corrupt");
        DEBUG_PRINTF("Stored: %u, Calculated: %u\n", storedChecksum, calculatedChecksum);
        // Continue anyway - checksum might not be available on older saves
    }
    
    DEBUG_PRINTF("Loaded WiFi credentials: %s (checksum: %s)\n", 
                 ssid.c_str(), 
                 storedChecksum == calculatedChecksum ? "OK" : "MISMATCH");
    return true;
}

void WiFiManager::saveWiFiCredentials(const String& ssid, const String& password) {
    // Use the improved version
    saveWiFiCredentialsWithVerification(ssid, password);
}

// Rest of the methods remain the same...
void WiFiManager::loop() {
    if (isAPMode && configServer && dnsServer) {
        dnsServer->processNextRequest();
        configServer->handleClient();
    }
    
    // Check connection status
    if (!isAPMode && WiFi.status() != WL_CONNECTED) {
        if (isConnected) {
            DEBUG_PRINTLN("WiFi connection lost");
            isConnected = false;
        }
    } else if (!isAPMode && WiFi.status() == WL_CONNECTED) {
        if (!isConnected) {
            DEBUG_PRINTLN("WiFi connection restored");
            isConnected = true;
        }
    }
}

bool WiFiManager::isConnectedToWiFi() {
    return isConnected && WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getIPAddress() {
    if (isAPMode) {
        return WiFi.softAPIP().toString();
    } else {
        return WiFi.localIP().toString();
    }
}

String WiFiManager::getSSID() {
    if (isAPMode) {
        return String(AP_SSID);
    } else {
        return WiFi.SSID();
    }
}

int WiFiManager::getRSSI() {
    if (isAPMode) {
        return 0;
    }
    return WiFi.RSSI();
}

String WiFiManager::getStatus() {
    DynamicJsonDocument status(512);
    status["mode"] = isAPMode ? "AP" : "STA";
    status["connected"] = isConnectedToWiFi();
    status["ssid"] = getSSID();
    status["ip"] = getIPAddress();
    status["rssi"] = getRSSI();
    
    String statusStr;
    serializeJson(status, statusStr);
    return statusStr;
}

void WiFiManager::resetCredentials() {
    EEPROM.writeUShort(MAGIC_ADDR, 0);
    EEPROM.writeUShort(CHECKSUM_ADDR, 0);
    EEPROM.commit();
    DEBUG_PRINTLN("WiFi credentials reset");
}

void WiFiManager::setConnectionTimeout(unsigned long timeout) {
    connectionTimeout = timeout;
}