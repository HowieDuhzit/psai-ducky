#include "wifi_manager.h"
#include "config.h"
#include <EEPROM.h>
#include <ArduinoJson.h>

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
    WiFi.mode(WIFI_STA);
    
    // Debug: Check EEPROM status
    DEBUG_PRINTF("EEPROM initialized, magic value: 0x%X\n", EEPROM.readUShort(MAGIC_ADDR));
    
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
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    
    DEBUG_PRINTF("AP started: %s\n", AP_SSID);
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
    String html = F("<!DOCTYPE html><html><head><title>ESP32 MCP Server - WiFi Setup</title>");
    html += F("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    html += F("<style>body{font-family:Arial;text-align:center;margin:50px}");
    html += F(".container{max-width:400px;margin:0 auto}");
    html += F("input[type=text],input[type=password]{width:100%;padding:10px;margin:10px 0;border:1px solid #ddd}");
    html += F("button{width:100%;padding:10px;background:#007cba;color:white;border:none;cursor:pointer}");
    html += F("button:hover{background:#005a8b}.info{background:#f0f0f0;padding:10px;margin:10px 0}");
    html += F("</style></head><body><div class='container'>");
    html += F("<h1>ESP32 MCP Server</h1><h2>WiFi Configuration</h2>");
    html += F("<div class='info'><p><strong>Device:</strong> ESP32 HID Controller</p>");
    html += F("<p><strong>Version:</strong> ");
    html += MCP_SERVER_VERSION;
    html += F("</p><p><strong>IP:</strong> ");
    html += WiFi.softAPIP().toString();
    html += F("</p></div>");
    html += F("<form action='/save' method='POST'><h3>WiFi Credentials</h3>");
    html += F("<input type='text' name='ssid' placeholder='WiFi Network Name (SSID)' required>");
    html += F("<input type='password' name='password' placeholder='WiFi Password'>");
    html += F("<button type='submit'>Save & Connect</button></form>");
    html += F("<div class='info'><a href='/debug'>Debug Info</a></div>"); // NEW: Debug link
    html += F("</div></body></html>");
    
    return html;
}

String WiFiManager::generateDebugHTML() {
    String ssid, password;
    bool hasCredentials = loadWiFiCredentials(ssid, password);
    
    String html = F("<!DOCTYPE html><html><head><title>WiFi Debug Info</title>");
    html += F("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    html += F("<style>body{font-family:Arial;margin:20px}.info{background:#f0f0f0;padding:10px;margin:10px 0}</style>");
    html += F("</head><body><h1>WiFi Debug Information</h1>");
    
    html += F("<div class='info'><h3>EEPROM Status</h3>");
    html += F("<p>Magic Value: 0x"); 
    html += String(EEPROM.readUShort(MAGIC_ADDR), HEX);
    html += F(" (Expected: 0xABCD)</p>");
    html += F("<p>Has Saved Credentials: "); 
    html += hasCredentials ? "YES" : "NO";
    html += F("</p>");
    
    if (hasCredentials) {
        html += F("<p>Saved SSID: ");
        html += ssid;
        html += F("</p><p>Password Length: ");
        html += String(password.length());
        html += F("</p>");
    }
    
    html += F("</div><div class='info'><h3>Current WiFi Status</h3>");
    html += F("<p>Mode: ");
    html += WiFi.getMode() == WIFI_AP ? "Access Point" : "Station";
    html += F("</p><p>Status: ");
    html += WiFi.status();
    html += F("</p>");
    
    if (WiFi.status() == WL_CONNECTED) {
        html += F("<p>Connected SSID: ");
        html += WiFi.SSID();
        html += F("</p><p>IP Address: ");
        html += WiFi.localIP().toString();
        html += F("</p>");
    }
    
    html += F("</div><p><a href='/'>Back to Config</a></p></body></html>");
    
    return html;
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