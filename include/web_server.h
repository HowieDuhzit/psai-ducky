#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

class ConfigWebServer {
public:
    ConfigWebServer(uint16_t port = 80);
    void begin();
    void loop();
    void stop();
    
    // Callbacks for accessing system info
    void setStatusCallback(String (*callback)());
    void setOTAStatusCallback(String (*callback)());
    void setMacroListCallback(String (*callback)());
    
private:
    WebServer* _server;
    uint16_t _port;
    bool _initialized;
    
    // Callback functions
    String (*_statusCallback)();
    String (*_otaStatusCallback)();
    String (*_macroListCallback)();
    
    // Page handlers
    void handleRoot();
    void handleConfig();
    void handleWiFi();
    void handleOTA();
    void handleMacros();
    void handleKeyboardLayout();
    void handleAPIStatus();
    void handleAPIWiFiScan();
    void handleAPIWiFiSave();
    void handleAPISystemRestart();
    void handleAPIFactoryReset();
    void handleNotFound();
    
    // HTML generators
    String generateHeader(const String& title);
    String generateFooter();
    String generateNav();
    String generateWiFiPage();
    String generateOTAPage();
    String generateMacrosPage();
    String generateLayoutPage();
};

#endif // WEB_SERVER_H
