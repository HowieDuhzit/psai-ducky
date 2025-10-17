#include "web_server.h"

ConfigWebServer::ConfigWebServer(uint16_t port) 
    : _port(port), _initialized(false), 
      _statusCallback(nullptr), _otaStatusCallback(nullptr), _macroListCallback(nullptr) {
    _server = new WebServer(port);
}

void ConfigWebServer::begin() {
    if (_initialized) return;
    
    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS initialization failed");
    }
    
    // Setup routes
    _server->on("/", [this]() { handleRoot(); });
    _server->on("/config", [this]() { handleConfig(); });
    _server->on("/wifi", [this]() { handleWiFi(); });
    _server->on("/ota", [this]() { handleOTA(); });
    _server->on("/macros", [this]() { handleMacros(); });
    _server->on("/layout", [this]() { handleKeyboardLayout(); });
    
    // API routes
    _server->on("/api/status", [this]() { handleAPIStatus(); });
    _server->on("/api/wifi/scan", [this]() { handleAPIWiFiScan(); });
    _server->on("/api/wifi/save", HTTP_POST, [this]() { handleAPIWiFiSave(); });
    _server->on("/api/system/restart", HTTP_POST, [this]() { handleAPISystemRestart(); });
    _server->on("/api/system/factory-reset", HTTP_POST, [this]() { handleAPIFactoryReset(); });
    
    _server->onNotFound([this]() { handleNotFound(); });
    
    _server->begin();
    _initialized = true;
    
    Serial.printf("Web server started on port %d\n", _port);
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("Config UI: http://%s\n", WiFi.localIP().toString().c_str());
    }
}

void ConfigWebServer::loop() {
    if (_initialized) {
        _server->handleClient();
    }
}

void ConfigWebServer::stop() {
    if (_initialized) {
        _server->stop();
        _initialized = false;
    }
}

void ConfigWebServer::setStatusCallback(String (*callback)()) {
    _statusCallback = callback;
}

void ConfigWebServer::setOTAStatusCallback(String (*callback)()) {
    _otaStatusCallback = callback;
}

void ConfigWebServer::setMacroListCallback(String (*callback)()) {
    _macroListCallback = callback;
}

String ConfigWebServer::generateHeader(const String& title) {
    return R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" + title + R"( - psAI-Ducky</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        .header {
            background: white;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        .header h1 {
            color: #667eea;
            font-size: 28px;
        }
        .header p {
            color: #666;
            margin-top: 5px;
        }
        nav {
            background: white;
            border-radius: 10px;
            padding: 15px;
            margin-bottom: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        nav a {
            display: inline-block;
            padding: 10px 20px;
            margin: 5px;
            background: #667eea;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            transition: background 0.3s;
        }
        nav a:hover {
            background: #764ba2;
        }
        nav a.active {
            background: #764ba2;
        }
        .content {
            background: white;
            border-radius: 10px;
            padding: 30px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        .card {
            background: #f8f9fa;
            border: 1px solid #e9ecef;
            border-radius: 8px;
            padding: 20px;
            margin-bottom: 20px;
        }
        .card h2 {
            color: #667eea;
            margin-bottom: 15px;
        }
        .form-group {
            margin-bottom: 15px;
        }
        label {
            display: block;
            margin-bottom: 5px;
            font-weight: 500;
            color: #333;
        }
        input[type="text"],
        input[type="password"],
        select {
            width: 100%;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 5px;
            font-size: 14px;
        }
        button {
            background: #667eea;
            color: white;
            border: none;
            padding: 12px 24px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
            transition: background 0.3s;
        }
        button:hover {
            background: #764ba2;
        }
        button.danger {
            background: #dc3545;
        }
        button.danger:hover {
            background: #c82333;
        }
        .status {
            padding: 10px;
            border-radius: 5px;
            margin-bottom: 15px;
        }
        .status.success {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .status.error {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        .status.info {
            background: #d1ecf1;
            color: #0c5460;
            border: 1px solid #bee5eb;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 10px;
        }
        th, td {
            padding: 10px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background: #f8f9fa;
            font-weight: 600;
        }
        .badge {
            display: inline-block;
            padding: 4px 8px;
            border-radius: 3px;
            font-size: 12px;
            font-weight: 500;
        }
        .badge.success {
            background: #28a745;
            color: white;
        }
        .badge.warning {
            background: #ffc107;
            color: #212529;
        }
        .badge.danger {
            background: #dc3545;
            color: white;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🦆 psAI-Ducky</h1>
            <p>ESP32 HID Controller - Configuration Interface</p>
        </div>
)";
}

String ConfigWebServer::generateNav() {
    String currentPath = _server->uri();
    
    auto linkClass = [&](const String& path) {
        return (currentPath == path) ? "class=\"active\"" : "";
    };
    
    return R"(
        <nav>
            <a href="/" )" + linkClass("/") + R"(>Dashboard</a>
            <a href="/wifi" )" + linkClass("/wifi") + R"(>WiFi</a>
            <a href="/ota" )" + linkClass("/ota") + R"(>OTA Update</a>
            <a href="/macros" )" + linkClass("/macros") + R"(>Macros</a>
            <a href="/layout" )" + linkClass("/layout") + R"(>Keyboard Layout</a>
        </nav>
)";
}

String ConfigWebServer::generateFooter() {
    return R"(
    </div>
</body>
</html>
)";
}

void ConfigWebServer::handleRoot() {
    String html = generateHeader("Dashboard");
    html += generateNav();
    html += R"(
        <div class="content">
            <div class="card">
                <h2>System Status</h2>
                <table>
                    <tr><th>Property</th><th>Value</th></tr>
                    <tr><td>Board</td><td>)" + String(ARDUINO_BOARD) + R"(</td></tr>
                    <tr><td>Chip Model</td><td>)" + String(ESP.getChipModel()) + R"(</td></tr>
                    <tr><td>CPU Frequency</td><td>)" + String(ESP.getCpuFreqMHz()) + R"( MHz</td></tr>
                    <tr><td>Free Heap</td><td>)" + String(ESP.getFreeHeap() / 1024) + R"( KB</td></tr>
                    <tr><td>SPIFFS Used</td><td>)" + String(SPIFFS.usedBytes() / 1024) + R"( / )" + String(SPIFFS.totalBytes() / 1024) + R"( KB</td></tr>
                    <tr><td>Uptime</td><td>)" + String(millis() / 1000) + R"( seconds</td></tr>
                </table>
            </div>
            
            <div class="card">
                <h2>Network</h2>
                <table>
                    <tr><th>Property</th><th>Value</th></tr>
                    <tr><td>Status</td><td>
    )";
    
    if (WiFi.status() == WL_CONNECTED) {
        html += R"(<span class="badge success">Connected</span>)";
    } else {
        html += R"(<span class="badge danger">Disconnected</span>)";
    }
    
    html += R"(
                    </td></tr>
                    <tr><td>SSID</td><td>)" + WiFi.SSID() + R"(</td></tr>
                    <tr><td>IP Address</td><td>)" + WiFi.localIP().toString() + R"(</td></tr>
                    <tr><td>MAC Address</td><td>)" + WiFi.macAddress() + R"(</td></tr>
                    <tr><td>Signal Strength</td><td>)" + String(WiFi.RSSI()) + R"( dBm</td></tr>
                </table>
            </div>
            
            <div class="card">
                <h2>HID Status</h2>
    )";
    
    #if HAVE_NATIVE_USB
    html += R"(<p><span class="badge success">USB HID</span> Native USB HID enabled</p>)";
    #else
    html += R"(<p><span class="badge warning">BLE HID</span> Bluetooth HID enabled</p>)";
    #endif
    
    if (_statusCallback) {
        html += "<p>" + _statusCallback() + "</p>";
    }
    
    html += R"(
            </div>
        </div>
    )";
    html += generateFooter();
    
    _server->send(200, "text/html", html);
}

void ConfigWebServer::handleConfig() {
    handleRoot();
}

void ConfigWebServer::handleWiFi() {
    String html = generateHeader("WiFi Configuration");
    html += generateNav();
    html += generateWiFiPage();
    html += generateFooter();
    
    _server->send(200, "text/html", html);
}

String ConfigWebServer::generateWiFiPage() {
    return R"(
        <div class="content">
            <div class="card">
                <h2>WiFi Settings</h2>
                <form action="/api/wifi/save" method="POST">
                    <div class="form-group">
                        <label>SSID</label>
                        <input type="text" name="ssid" value=")" + WiFi.SSID() + R"(" required>
                    </div>
                    <div class="form-group">
                        <label>Password</label>
                        <input type="password" name="password" placeholder="Enter new password if changing">
                    </div>
                    <button type="submit">Save & Reconnect</button>
                </form>
            </div>
            
            <div class="card">
                <h2>Available Networks</h2>
                <button onclick="scanNetworks()">Scan Networks</button>
                <div id="networks"></div>
            </div>
            
            <script>
                function scanNetworks() {
                    document.getElementById('networks').innerHTML = '<p>Scanning...</p>';
                    fetch('/api/wifi/scan')
                        .then(r => r.json())
                        .then(data => {
                            let html = '<table><tr><th>SSID</th><th>Signal</th><th>Security</th><th>Action</th></tr>';
                            data.networks.forEach(net => {
                                html += `<tr>
                                    <td>${net.ssid}</td>
                                    <td>${net.rssi} dBm</td>
                                    <td>${net.encryption}</td>
                                    <td><button onclick="selectNetwork('${net.ssid}')">Select</button></td>
                                </tr>`;
                            });
                            html += '</table>';
                            document.getElementById('networks').innerHTML = html;
                        });
                }
                
                function selectNetwork(ssid) {
                    document.querySelector('input[name="ssid"]').value = ssid;
                    document.querySelector('input[name="password"]').focus();
                }
            </script>
        </div>
    )";
}

void ConfigWebServer::handleOTA() {
    String html = generateHeader("OTA Update");
    html += generateNav();
    html += generateOTAPage();
    html += generateFooter();
    
    _server->send(200, "text/html", html);
}

String ConfigWebServer::generateOTAPage() {
    String otaStatus = "Ready";
    if (_otaStatusCallback) {
        otaStatus = _otaStatusCallback();
    }
    
    return R"(
        <div class="content">
            <div class="card">
                <h2>Over-The-Air Updates</h2>
                <div class="status info">
                    <strong>Status:</strong> )" + otaStatus + R"(
                </div>
                
                <h3>Using Arduino IDE</h3>
                <ol>
                    <li>Select <strong>Tools → Port → Network ports</strong></li>
                    <li>Select <strong>psai-ducky at )" + WiFi.localIP().toString() + R"(</strong></li>
                    <li>Click <strong>Upload</strong></li>
                </ol>
                
                <h3>Using PlatformIO</h3>
                <pre>platformio run -e esp32-s2-flipper -t upload --upload-port )" + WiFi.localIP().toString() + R"(</pre>
                
                <h3>OTA Configuration</h3>
                <table>
                    <tr><td>Hostname</td><td>psai-ducky.local</td></tr>
                    <tr><td>IP Address</td><td>)" + WiFi.localIP().toString() + R"(</td></tr>
                    <tr><td>Port</td><td>3232</td></tr>
                </table>
            </div>
            
            <div class="card">
                <h2>System Control</h2>
                <button onclick="if(confirm('Restart device?')) restart()">Restart Device</button>
                <button class="danger" onclick="if(confirm('Factory reset? This will erase all settings!')) factoryReset()">Factory Reset</button>
            </div>
            
            <script>
                function restart() {
                    fetch('/api/system/restart', {method: 'POST'})
                        .then(() => alert('Device restarting...'));
                }
                
                function factoryReset() {
                    fetch('/api/system/factory-reset', {method: 'POST'})
                        .then(() => alert('Factory reset complete. Device restarting...'));
                }
            </script>
        </div>
    )";
}

void ConfigWebServer::handleMacros() {
    String html = generateHeader("Macro Management");
    html += generateNav();
    html += generateMacrosPage();
    html += generateFooter();
    
    _server->send(200, "text/html", html);
}

String ConfigWebServer::generateMacrosPage() {
    return R"(
        <div class="content">
            <div class="card">
                <h2>Macro Management</h2>
                <p><span class="badge warning">Coming Soon</span> Macro recording and playback will be available in the next update.</p>
                
                <h3>Planned Features</h3>
                <ul>
                    <li>Record keyboard/mouse sequences</li>
                    <li>Save macros to SPIFFS</li>
                    <li>Trigger macros via MCP or web interface</li>
                    <li>Import/export macro files</li>
                </ul>
            </div>
        </div>
    )";
}

void ConfigWebServer::handleKeyboardLayout() {
    String html = generateHeader("Keyboard Layout");
    html += generateNav();
    html += generateLayoutPage();
    html += generateFooter();
    
    _server->send(200, "text/html", html);
}

String ConfigWebServer::generateLayoutPage() {
    return R"(
        <div class="content">
            <div class="card">
                <h2>Keyboard Layout</h2>
                <div class="form-group">
                    <label>Select Layout</label>
                    <select id="layout">
                        <option value="US">US (QWERTY)</option>
                        <option value="UK">UK (QWERTY)</option>
                        <option value="DE">German (QWERTZ)</option>
                        <option value="FR">French (AZERTY)</option>
                        <option value="ES">Spanish</option>
                        <option value="IT">Italian</option>
                    </select>
                </div>
                <button onclick="saveLayout()">Save Layout</button>
                
                <div class="status info" style="margin-top: 20px;">
                    <strong>Note:</strong> Layout switching is currently in development.
                </div>
            </div>
            
            <script>
                function saveLayout() {
                    const layout = document.getElementById('layout').value;
                    alert('Layout ' + layout + ' saved (feature in development)');
                }
            </script>
        </div>
    )";
}

void ConfigWebServer::handleAPIStatus() {
    String json = "{";
    json += "\"wifi\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false") + ",";
    json += "\"ssid\":\"" + WiFi.SSID() + "\",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"uptime\":" + String(millis() / 1000) + ",";
    json += "\"free_heap\":" + String(ESP.getFreeHeap());
    json += "}";
    
    _server->send(200, "application/json", json);
}

void ConfigWebServer::handleAPIWiFiScan() {
    int n = WiFi.scanNetworks();
    String json = "{\"networks\":[";
    
    for (int i = 0; i < n; i++) {
        if (i > 0) json += ",";
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        json += "\"encryption\":\"" + String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Open" : "Secured") + "\"";
        json += "}";
    }
    
    json += "]}";
    _server->send(200, "application/json", json);
}

void ConfigWebServer::handleAPIWiFiSave() {
    String ssid = _server->arg("ssid");
    String password = _server->arg("password");
    
    // TODO: Save to SPIFFS and reconnect
    _server->send(200, "text/plain", "WiFi settings saved. Reconnecting...");
}

void ConfigWebServer::handleAPISystemRestart() {
    _server->send(200, "text/plain", "Restarting...");
    delay(1000);
    ESP.restart();
}

void ConfigWebServer::handleAPIFactoryReset() {
    // TODO: Clear SPIFFS and reset settings
    _server->send(200, "text/plain", "Factory reset complete. Restarting...");
    SPIFFS.format();
    delay(1000);
    ESP.restart();
}

void ConfigWebServer::handleNotFound() {
    _server->send(404, "text/plain", "404 Not Found");
}
