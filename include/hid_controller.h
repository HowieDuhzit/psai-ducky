#ifndef HID_CONTROLLER_H
#define HID_CONTROLLER_H

#include <Arduino.h>
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

// Mouse button definitions
#define MOUSE_LEFT 0x01
#define MOUSE_RIGHT 0x02
#define MOUSE_MIDDLE 0x04

// Special key definitions
#define KEY_CTRL 0x01
#define KEY_SHIFT 0x02
#define KEY_ALT 0x04
#define KEY_GUI 0x08

class HIDController {
private:
    USBHIDKeyboard* keyboard;
    USBHIDMouse* mouse;
    bool isInitialized;
    
    // Key mapping functions
    uint8_t mapSpecialKey(const String& keyName);
    uint8_t mapMouseButton(const String& buttonName);
    
public:
    HIDController(USBHIDKeyboard* kb, USBHIDMouse* ms);
    ~HIDController();
    
    bool begin();
    
    // Keyboard functions
    bool typeText(const String& text);
    bool pressKey(uint8_t key, uint8_t modifiers = 0);
    bool releaseKey(uint8_t key, uint8_t modifiers = 0);
    bool sendKeyStroke(uint8_t key, uint8_t modifiers = 0);
    bool sendKeyStroke(const String& keyName, const String& modifiers = "");
    bool sendKeySequence(const String& sequence);
    
    // Special key combinations
    bool sendCtrlC();
    bool sendCtrlV();
    bool sendCtrlZ();
    bool sendAltTab();
    bool sendWinKey();
    
    // Mouse functions
    bool moveMouse(int16_t x, int16_t y, bool relative = true);
    bool clickMouse(uint8_t button = MOUSE_LEFT, uint16_t duration = 50);
    bool doubleClickMouse(uint8_t button = MOUSE_LEFT);
    bool pressMouse(uint8_t button);
    bool releaseMouse(uint8_t button);
    bool scrollMouse(int8_t scroll);
    
    // System functions
    bool isReady();
    void reset();
    String getStatus();
};

#endif // HID_CONTROLLER_H