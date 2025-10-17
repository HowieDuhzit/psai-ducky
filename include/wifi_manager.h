#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

class WiFiManager {
private:
    WebServer* configServer;
    DNSServer* dnsServer;
    bool isAPMode;
    bool isConnected;
    unsigned long lastConnectionAttempt;
    unsigned long connectionTimeout;
    
    // Configuration portal
    void setupConfigPortal();
    void handleConfigPage();
    void handleConfigSave();
    void handleNotFound();
    void handleDebugPage();
    String generateConfigHTML();
    String generateDebugHTML();
    
    // Connection management
    bool connectToWiFi(const String& ssid, const String& password);
    void saveWiFiCredentials(const String& ssid, const String& password);
    bool saveWiFiCredentialsWithVerification(const String& ssid, const String& password);
    bool loadWiFiCredentials(String& ssid, String& password);
    
public:
    WiFiManager();
    ~WiFiManager();
    
    bool begin();
    bool connect();
    bool connect(const String& ssid, const String& password);
    void startAccessPoint();
    void loop();
    
    // Status functions
    bool isConnectedToWiFi();
    String getIPAddress();
    String getSSID();
    int getRSSI();
    String getStatus();
    
    // Configuration
    void resetCredentials();
    void setConnectionTimeout(unsigned long timeout);
};

#endif // WIFI_MANAGER_H