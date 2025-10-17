#include "macro_manager.h"

#define MACRO_DIR "/macros"

MacroManager::MacroManager() 
    : _initialized(false), _recording(false), _playing(false),
      _playingMacro(nullptr), _recordStartTime(0), _playStartTime(0),
      _playbackIndex(0), _playbackLoop(false) {
}

void MacroManager::begin() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
        return;
    }
    
    // Create macros directory if it doesn't exist
    if (!SPIFFS.exists(MACRO_DIR)) {
        SPIFFS.mkdir(MACRO_DIR);
    }
    
    _initialized = true;
    Serial.println("Macro Manager initialized");
    Serial.printf("Macro storage: %s\n", MACRO_DIR);
}

void MacroManager::startRecording(const String& name) {
    if (_recording) {
        Serial.println("Already recording");
        return;
    }
    
    clearCurrentMacro();
    _currentMacro.name = name;
    _currentMacro.loop = false;
    _recording = true;
    _recordStartTime = millis();
    
    Serial.printf("Started recording macro '%s'\n", name.c_str());
}

void MacroManager::stopRecording() {
    if (!_recording) {
        Serial.println("Not recording");
        return;
    }
    
    _recording = false;
    _currentMacro.totalDuration = millis() - _recordStartTime;
    
    Serial.printf("Stopped recording macro '%s' (%d actions, %dms)\n", 
                  _currentMacro.name.c_str(), 
                  _currentMacro.actions.size(),
                  _currentMacro.totalDuration);
    
    // Auto-save
    saveMacro(_currentMacro.name);
}

bool MacroManager::isRecording() {
    return _recording;
}

void MacroManager::recordKeyPress(uint8_t modifiers, uint8_t key) {
    if (!_recording) return;
    
    MacroAction action;
    action.type = ACTION_KEY_PRESS;
    action.timestamp = millis() - _recordStartTime;
    action.data.key.modifiers = modifiers;
    action.data.key.key = key;
    
    _currentMacro.actions.push_back(action);
    Serial.printf("Recorded key press: mod=0x%02X key=0x%02X @%dms\n", 
                  modifiers, key, action.timestamp);
}

void MacroManager::recordKeyRelease(uint8_t key) {
    if (!_recording) return;
    
    MacroAction action;
    action.type = ACTION_KEY_RELEASE;
    action.timestamp = millis() - _recordStartTime;
    action.data.key.key = key;
    action.data.key.modifiers = 0;
    
    _currentMacro.actions.push_back(action);
    Serial.printf("Recorded key release: key=0x%02X @%dms\n", 
                  key, action.timestamp);
}

void MacroManager::recordMouseMove(int16_t x, int16_t y) {
    if (!_recording) return;
    
    MacroAction action;
    action.type = ACTION_MOUSE_MOVE;
    action.timestamp = millis() - _recordStartTime;
    action.data.mouse.x = x;
    action.data.mouse.y = y;
    
    _currentMacro.actions.push_back(action);
    Serial.printf("Recorded mouse move: x=%d y=%d @%dms\n", 
                  x, y, action.timestamp);
}

void MacroManager::recordMouseClick(uint8_t button) {
    if (!_recording) return;
    
    MacroAction action;
    action.type = ACTION_MOUSE_CLICK;
    action.timestamp = millis() - _recordStartTime;
    action.data.click.button = button;
    
    _currentMacro.actions.push_back(action);
    Serial.printf("Recorded mouse click: button=%d @%dms\n", 
                  button, action.timestamp);
}

void MacroManager::recordMouseScroll(int8_t amount) {
    if (!_recording) return;
    
    MacroAction action;
    action.type = ACTION_MOUSE_SCROLL;
    action.timestamp = millis() - _recordStartTime;
    action.data.scroll.amount = amount;
    
    _currentMacro.actions.push_back(action);
    Serial.printf("Recorded mouse scroll: amount=%d @%dms\n", 
                  amount, action.timestamp);
}

void MacroManager::recordDelay(uint32_t duration) {
    if (!_recording) return;
    
    MacroAction action;
    action.type = ACTION_DELAY;
    action.timestamp = millis() - _recordStartTime;
    action.data.delay.duration = duration;
    
    _currentMacro.actions.push_back(action);
    Serial.printf("Recorded delay: %dms @%dms\n", 
                  duration, action.timestamp);
}

bool MacroManager::playMacro(const String& name, bool loop) {
    if (_playing) {
        Serial.println("Already playing a macro");
        return false;
    }
    
    // Find macro
    Macro* macro = getMacro(name);
    if (!macro) {
        // Try loading from file
        if (!loadMacro(name)) {
            Serial.printf("Macro '%s' not found\n", name.c_str());
            return false;
        }
        macro = getMacro(name);
    }
    
    if (!macro || macro->actions.empty()) {
        Serial.printf("Macro '%s' is empty\n", name.c_str());
        return false;
    }
    
    _playingMacro = macro;
    _playing = true;
    _playbackLoop = loop;
    _playbackIndex = 0;
    _playStartTime = millis();
    
    Serial.printf("Started playing macro '%s' (%d actions, %dms%s)\n", 
                  name.c_str(), 
                  macro->actions.size(),
                  macro->totalDuration,
                  loop ? ", looping" : "");
    
    return true;
}

void MacroManager::stopPlayback() {
    if (!_playing) return;
    
    _playing = false;
    _playingMacro = nullptr;
    _playbackIndex = 0;
    
    Serial.println("Stopped macro playback");
}

bool MacroManager::isPlaying() {
    return _playing;
}

void MacroManager::update() {
    if (!_playing || !_playingMacro) return;
    
    uint32_t elapsed = millis() - _playStartTime;
    
    // Process actions that should have happened by now
    while (_playbackIndex < _playingMacro->actions.size()) {
        const MacroAction& action = _playingMacro->actions[_playbackIndex];
        
        if (action.timestamp > elapsed) {
            // Not time for this action yet
            break;
        }
        
        // Execute action
        switch (action.type) {
            case ACTION_KEY_PRESS:
                // TODO: Call HID controller keypress
                Serial.printf("Macro: Key press mod=0x%02X key=0x%02X\n", 
                              action.data.key.modifiers, action.data.key.key);
                break;
                
            case ACTION_KEY_RELEASE:
                // TODO: Call HID controller key release
                Serial.printf("Macro: Key release key=0x%02X\n", 
                              action.data.key.key);
                break;
                
            case ACTION_MOUSE_MOVE:
                // TODO: Call HID controller mouse move
                Serial.printf("Macro: Mouse move x=%d y=%d\n", 
                              action.data.mouse.x, action.data.mouse.y);
                break;
                
            case ACTION_MOUSE_CLICK:
                // TODO: Call HID controller mouse click
                Serial.printf("Macro: Mouse click button=%d\n", 
                              action.data.click.button);
                break;
                
            case ACTION_MOUSE_SCROLL:
                // TODO: Call HID controller mouse scroll
                Serial.printf("Macro: Mouse scroll amount=%d\n", 
                              action.data.scroll.amount);
                break;
                
            case ACTION_DELAY:
                Serial.printf("Macro: Delay %dms\n", 
                              action.data.delay.duration);
                break;
        }
        
        _playbackIndex++;
    }
    
    // Check if finished
    if (_playbackIndex >= _playingMacro->actions.size()) {
        if (_playbackLoop) {
            // Restart
            _playbackIndex = 0;
            _playStartTime = millis();
            Serial.println("Macro: Restarting loop");
        } else {
            stopPlayback();
        }
    }
}

bool MacroManager::saveMacro(const String& name) {
    if (!_initialized) return false;
    
    String path = getMacroPath(name);
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file) {
        Serial.printf("Failed to open file for writing: %s\n", path.c_str());
        return false;
    }
    
    // Create JSON document
    DynamicJsonDocument doc(8192);
    doc["name"] = _currentMacro.name;
    doc["description"] = _currentMacro.description;
    doc["totalDuration"] = _currentMacro.totalDuration;
    doc["loop"] = _currentMacro.loop;
    
    JsonArray actions = doc.createNestedArray("actions");
    for (const auto& action : _currentMacro.actions) {
        JsonObject actionObj = actions.createNestedObject();
        actionObj["type"] = action.type;
        actionObj["timestamp"] = action.timestamp;
        
        switch (action.type) {
            case ACTION_KEY_PRESS:
            case ACTION_KEY_RELEASE:
                actionObj["modifiers"] = action.data.key.modifiers;
                actionObj["key"] = action.data.key.key;
                break;
            case ACTION_MOUSE_MOVE:
                actionObj["x"] = action.data.mouse.x;
                actionObj["y"] = action.data.mouse.y;
                break;
            case ACTION_MOUSE_CLICK:
                actionObj["button"] = action.data.click.button;
                break;
            case ACTION_MOUSE_SCROLL:
                actionObj["amount"] = action.data.scroll.amount;
                break;
            case ACTION_DELAY:
                actionObj["duration"] = action.data.delay.duration;
                break;
        }
    }
    
    serializeJson(doc, file);
    file.close();
    
    // Add to loaded macros
    _macros.push_back(_currentMacro);
    
    Serial.printf("Saved macro '%s' to %s\n", name.c_str(), path.c_str());
    return true;
}

bool MacroManager::loadMacro(const String& name) {
    if (!_initialized) return false;
    
    String path = getMacroPath(name);
    if (!SPIFFS.exists(path)) {
        Serial.printf("Macro file not found: %s\n", path.c_str());
        return false;
    }
    
    File file = SPIFFS.open(path, FILE_READ);
    if (!file) {
        Serial.printf("Failed to open file for reading: %s\n", path.c_str());
        return false;
    }
    
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.printf("Failed to parse macro file: %s\n", error.c_str());
        return false;
    }
    
    Macro macro;
    macro.name = doc["name"].as<String>();
    macro.description = doc["description"].as<String>();
    macro.totalDuration = doc["totalDuration"];
    macro.loop = doc["loop"];
    
    JsonArray actions = doc["actions"];
    for (JsonObject actionObj : actions) {
        MacroAction action;
        action.type = (MacroActionType)actionObj["type"].as<int>();
        action.timestamp = actionObj["timestamp"];
        
        switch (action.type) {
            case ACTION_KEY_PRESS:
            case ACTION_KEY_RELEASE:
                action.data.key.modifiers = actionObj["modifiers"];
                action.data.key.key = actionObj["key"];
                break;
            case ACTION_MOUSE_MOVE:
                action.data.mouse.x = actionObj["x"];
                action.data.mouse.y = actionObj["y"];
                break;
            case ACTION_MOUSE_CLICK:
                action.data.click.button = actionObj["button"];
                break;
            case ACTION_MOUSE_SCROLL:
                action.data.scroll.amount = actionObj["amount"];
                break;
            case ACTION_DELAY:
                action.data.delay.duration = actionObj["duration"];
                break;
        }
        
        macro.actions.push_back(action);
    }
    
    _macros.push_back(macro);
    
    Serial.printf("Loaded macro '%s' (%d actions)\n", 
                  name.c_str(), macro.actions.size());
    return true;
}

bool MacroManager::deleteMacro(const String& name) {
    if (!_initialized) return false;
    
    String path = getMacroPath(name);
    if (!SPIFFS.exists(path)) {
        return false;
    }
    
    SPIFFS.remove(path);
    
    // Remove from loaded macros
    for (auto it = _macros.begin(); it != _macros.end(); ++it) {
        if (it->name == name) {
            _macros.erase(it);
            break;
        }
    }
    
    Serial.printf("Deleted macro '%s'\n", name.c_str());
    return true;
}

std::vector<String> MacroManager::listMacros() {
    std::vector<String> list;
    
    File root = SPIFFS.open(MACRO_DIR);
    if (!root || !root.isDirectory()) {
        return list;
    }
    
    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String filename = file.name();
            if (filename.endsWith(".json")) {
                String name = filename.substring(0, filename.length() - 5);
                list.push_back(name);
            }
        }
        file = root.openNextFile();
    }
    
    return list;
}

Macro* MacroManager::getMacro(const String& name) {
    for (auto& macro : _macros) {
        if (macro.name == name) {
            return &macro;
        }
    }
    return nullptr;
}

String MacroManager::exportMacro(const String& name) {
    Macro* macro = getMacro(name);
    if (!macro) {
        return "";
    }
    
    DynamicJsonDocument doc(8192);
    doc["name"] = macro->name;
    doc["description"] = macro->description;
    doc["totalDuration"] = macro->totalDuration;
    doc["loop"] = macro->loop;
    
    JsonArray actions = doc.createNestedArray("actions");
    for (const auto& action : macro->actions) {
        JsonObject actionObj = actions.createNestedObject();
        actionObj["type"] = action.type;
        actionObj["timestamp"] = action.timestamp;
        
        switch (action.type) {
            case ACTION_KEY_PRESS:
            case ACTION_KEY_RELEASE:
                actionObj["modifiers"] = action.data.key.modifiers;
                actionObj["key"] = action.data.key.key;
                break;
            case ACTION_MOUSE_MOVE:
                actionObj["x"] = action.data.mouse.x;
                actionObj["y"] = action.data.mouse.y;
                break;
            case ACTION_MOUSE_CLICK:
                actionObj["button"] = action.data.click.button;
                break;
            case ACTION_MOUSE_SCROLL:
                actionObj["amount"] = action.data.scroll.amount;
                break;
            case ACTION_DELAY:
                actionObj["duration"] = action.data.delay.duration;
                break;
        }
    }
    
    String output;
    serializeJson(doc, output);
    return output;
}

bool MacroManager::importMacro(const String& jsonData) {
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, jsonData);
    
    if (error) {
        Serial.printf("Failed to parse imported macro: %s\n", error.c_str());
        return false;
    }
    
    Macro macro;
    macro.name = doc["name"].as<String>();
    macro.description = doc["description"].as<String>();
    macro.totalDuration = doc["totalDuration"];
    macro.loop = doc["loop"];
    
    JsonArray actions = doc["actions"];
    for (JsonObject actionObj : actions) {
        MacroAction action;
        action.type = (MacroActionType)actionObj["type"].as<int>();
        action.timestamp = actionObj["timestamp"];
        
        switch (action.type) {
            case ACTION_KEY_PRESS:
            case ACTION_KEY_RELEASE:
                action.data.key.modifiers = actionObj["modifiers"];
                action.data.key.key = actionObj["key"];
                break;
            case ACTION_MOUSE_MOVE:
                action.data.mouse.x = actionObj["x"];
                action.data.mouse.y = actionObj["y"];
                break;
            case ACTION_MOUSE_CLICK:
                action.data.click.button = actionObj["button"];
                break;
            case ACTION_MOUSE_SCROLL:
                action.data.scroll.amount = actionObj["amount"];
                break;
            case ACTION_DELAY:
                action.data.delay.duration = actionObj["duration"];
                break;
        }
        
        macro.actions.push_back(action);
    }
    
    _currentMacro = macro;
    saveMacro(macro.name);
    
    Serial.printf("Imported macro '%s'\n", macro.name.c_str());
    return true;
}

String MacroManager::getStatus() {
    if (!_initialized) {
        return "Macro Manager not initialized";
    }
    
    String status = "Macros: ";
    status += String(_macros.size()) + " loaded";
    
    if (_recording) {
        uint32_t elapsed = (millis() - _recordStartTime) / 1000;
        status += ", RECORDING '" + _currentMacro.name + "' (" + String(elapsed) + "s)";
    }
    
    if (_playing && _playingMacro) {
        status += ", PLAYING '" + _playingMacro->name + "'";
        if (_playbackLoop) {
            status += " (loop)";
        }
    }
    
    return status;
}

void MacroManager::clearCurrentMacro() {
    _currentMacro.name = "";
    _currentMacro.description = "";
    _currentMacro.actions.clear();
    _currentMacro.totalDuration = 0;
    _currentMacro.loop = false;
}

String MacroManager::getMacroPath(const String& name) {
    return String(MACRO_DIR) + "/" + name + ".json";
}

bool MacroManager::macroExists(const String& name) {
    return SPIFFS.exists(getMacroPath(name));
}
