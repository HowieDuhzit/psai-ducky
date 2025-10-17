#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>

class OTAManager {
public:
    OTAManager();
    void begin(const char* hostname = "psai-ducky", const char* password = nullptr);
    void loop();
    void setPassword(const char* password);
    bool isUpdating();
    int getProgress();
    String getStatus();
    
private:
    bool _initialized;
    bool _updating;
    int _progress;
    String _status;
    String _hostname;
    
    static void onStart();
    static void onEnd();
    static void onProgress(unsigned int progress, unsigned int total);
    static void onError(ota_error_t error);
    
    // Static pointer to access instance in callbacks
    static OTAManager* _instance;
};

#endif // OTA_MANAGER_H
