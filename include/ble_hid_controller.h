#ifndef BLE_HID_CONTROLLER_H
#define BLE_HID_CONTROLLER_H

#include <Arduino.h>

// Only compile BLE HID for boards without native USB
#if !HAVE_NATIVE_USB

#include <BleKeyboard.h>
#include <BleMouse.h>

class BLEHIDController {
public:
    BLEHIDController(const char* deviceName = "psAI-Ducky");
    void begin();
    bool isConnected();
    
    // Keyboard methods
    void typeText(const String& text);
    void pressKey(uint8_t key);
    void releaseKey(uint8_t key);
    void pressKeys(uint8_t modifiers, uint8_t key);
    
    // Mouse methods
    void mouseMove(int x, int y);
    void mouseClick(uint8_t button = MOUSE_LEFT);
    void mouseScroll(int amount);
    
    String getStatus();

private:
    BleKeyboard* _keyboard;
    BleMouse* _mouse;
    String _deviceName;
    bool _initialized;
};

#else

// Stub for USB boards
class BLEHIDController {
public:
    BLEHIDController(const char* deviceName = "psAI-Ducky") {}
    void begin() {}
    bool isConnected() { return false; }
    void typeText(const String& text) {}
    void pressKey(uint8_t key) {}
    void releaseKey(uint8_t key) {}
    void pressKeys(uint8_t modifiers, uint8_t key) {}
    void mouseMove(int x, int y) {}
    void mouseClick(uint8_t button = 0) {}
    void mouseScroll(int amount) {}
    String getStatus() { return "BLE not available (native USB board)"; }
};

#endif

#endif // BLE_HID_CONTROLLER_H
