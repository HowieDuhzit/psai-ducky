#include "ota_manager.h"

// Static instance pointer
OTAManager* OTAManager::_instance = nullptr;

OTAManager::OTAManager() : _initialized(false), _updating(false), _progress(0), _status("Ready") {
    _instance = this;
}

void OTAManager::begin(const char* hostname, const char* password) {
    _hostname = String(hostname);
    
    // Set hostname
    ArduinoOTA.setHostname(hostname);
    
    // Set password if provided
    if (password != nullptr && strlen(password) > 0) {
        ArduinoOTA.setPassword(password);
    }
    
    // Configure OTA callbacks
    ArduinoOTA.onStart([]() {
        if (_instance) {
            _instance->_updating = true;
            _instance->_progress = 0;
            
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) {
                type = "sketch";
            } else { // U_SPIFFS
                type = "filesystem";
            }
            _instance->_status = "OTA Update Started: " + type;
            Serial.println(_instance->_status);
            
            // Stop any running services if needed
        }
    });
    
    ArduinoOTA.onEnd([]() {
        if (_instance) {
            _instance->_updating = false;
            _instance->_progress = 100;
            _instance->_status = "OTA Update Complete";
            Serial.println("\nOTA Update Complete!");
        }
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        if (_instance) {
            _instance->_progress = (progress / (total / 100));
            Serial.printf("OTA Progress: %u%%\r", _instance->_progress);
        }
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        if (_instance) {
            _instance->_updating = false;
            _instance->_status = "OTA Error: ";
            
            if (error == OTA_AUTH_ERROR) {
                _instance->_status += "Auth Failed";
            } else if (error == OTA_BEGIN_ERROR) {
                _instance->_status += "Begin Failed";
            } else if (error == OTA_CONNECT_ERROR) {
                _instance->_status += "Connect Failed";
            } else if (error == OTA_RECEIVE_ERROR) {
                _instance->_status += "Receive Failed";
            } else if (error == OTA_END_ERROR) {
                _instance->_status += "End Failed";
            }
            
            Serial.println(_instance->_status);
        }
    });
    
    ArduinoOTA.begin();
    _initialized = true;
    
    Serial.println("OTA Manager initialized");
    Serial.printf("OTA Hostname: %s.local\n", hostname);
}

void OTAManager::loop() {
    if (_initialized) {
        ArduinoOTA.handle();
    }
}

void OTAManager::setPassword(const char* password) {
    if (password != nullptr && strlen(password) > 0) {
        ArduinoOTA.setPassword(password);
    }
}

bool OTAManager::isUpdating() {
    return _updating;
}

int OTAManager::getProgress() {
    return _progress;
}

String OTAManager::getStatus() {
    return _status;
}
