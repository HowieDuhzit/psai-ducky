#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// Macro action types
enum MacroActionType {
    ACTION_KEY_PRESS,
    ACTION_KEY_RELEASE,
    ACTION_MOUSE_MOVE,
    ACTION_MOUSE_CLICK,
    ACTION_MOUSE_SCROLL,
    ACTION_DELAY
};

// Single macro action
struct MacroAction {
    MacroActionType type;
    uint32_t timestamp;  // Relative to macro start (ms)
    union {
        struct {
            uint8_t modifiers;
            uint8_t key;
        } key;
        struct {
            int16_t x;
            int16_t y;
        } mouse;
        struct {
            uint8_t button;
        } click;
        struct {
            int8_t amount;
        } scroll;
        struct {
            uint32_t duration;
        } delay;
    } data;
};

// Macro definition
struct Macro {
    String name;
    String description;
    std::vector<MacroAction> actions;
    uint32_t totalDuration;
    bool loop;
};

class MacroManager {
public:
    MacroManager();
    void begin();
    
    // Recording
    void startRecording(const String& name);
    void stopRecording();
    bool isRecording();
    void recordKeyPress(uint8_t modifiers, uint8_t key);
    void recordKeyRelease(uint8_t key);
    void recordMouseMove(int16_t x, int16_t y);
    void recordMouseClick(uint8_t button);
    void recordMouseScroll(int8_t amount);
    void recordDelay(uint32_t duration);
    
    // Playback
    bool playMacro(const String& name, bool loop = false);
    void stopPlayback();
    bool isPlaying();
    void update();  // Call in loop() to handle playback
    
    // Management
    bool saveMacro(const String& name);
    bool loadMacro(const String& name);
    bool deleteMacro(const String& name);
    std::vector<String> listMacros();
    Macro* getMacro(const String& name);
    
    // Export/Import
    String exportMacro(const String& name);
    bool importMacro(const String& jsonData);
    
    String getStatus();

private:
    bool _initialized;
    bool _recording;
    bool _playing;
    
    Macro _currentMacro;
    Macro* _playingMacro;
    uint32_t _recordStartTime;
    uint32_t _playStartTime;
    size_t _playbackIndex;
    bool _playbackLoop;
    
    std::vector<Macro> _macros;
    
    void clearCurrentMacro();
    String getMacroPath(const String& name);
    bool macroExists(const String& name);
};

#endif // MACRO_MANAGER_H
