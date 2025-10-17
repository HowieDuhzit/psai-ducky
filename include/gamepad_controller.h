#ifndef GAMEPAD_CONTROLLER_H
#define GAMEPAD_CONTROLLER_H

#include <Arduino.h>

#if HAVE_NATIVE_USB
#include "USB.h"
#include "USBHIDGamepad.h"

// Gamepad button definitions
#define GAMEPAD_BUTTON_A      0x01
#define GAMEPAD_BUTTON_B      0x02
#define GAMEPAD_BUTTON_X      0x04
#define GAMEPAD_BUTTON_Y      0x08
#define GAMEPAD_BUTTON_LB     0x10
#define GAMEPAD_BUTTON_RB     0x20
#define GAMEPAD_BUTTON_SELECT 0x40
#define GAMEPAD_BUTTON_START  0x80
#define GAMEPAD_BUTTON_L3     0x100
#define GAMEPAD_BUTTON_R3     0x200

// D-pad directions
#define DPAD_CENTERED  0x0F
#define DPAD_UP        0x00
#define DPAD_UP_RIGHT  0x01
#define DPAD_RIGHT     0x02
#define DPAD_DOWN_RIGHT 0x03
#define DPAD_DOWN      0x04
#define DPAD_DOWN_LEFT 0x05
#define DPAD_LEFT      0x06
#define DPAD_UP_LEFT   0x07

class GamepadController {
public:
    GamepadController();
    void begin();
    
    // Button controls
    void pressButton(uint16_t button);
    void releaseButton(uint16_t button);
    void pressButtons(uint16_t buttons);
    void releaseAllButtons();
    
    // Analog stick controls (-127 to 127)
    void setLeftStick(int8_t x, int8_t y);
    void setRightStick(int8_t x, int8_t y);
    void centerSticks();
    
    // D-pad control
    void setDPad(uint8_t direction);
    
    // Trigger controls (0 to 255)
    void setLeftTrigger(uint8_t value);
    void setRightTrigger(uint8_t value);
    
    // Send current state
    void sendReport();
    
    // Get status
    String getStatus();
    bool isInitialized();

private:
    USBHIDGamepad* _gamepad;
    bool _initialized;
    
    // Current state
    uint16_t _buttons;
    int8_t _leftX, _leftY;
    int8_t _rightX, _rightY;
    uint8_t _leftTrigger, _rightTrigger;
    uint8_t _dpad;
};

#else

// Stub for non-USB boards
class GamepadController {
public:
    GamepadController() {}
    void begin() {}
    void pressButton(uint16_t button) {}
    void releaseButton(uint16_t button) {}
    void pressButtons(uint16_t buttons) {}
    void releaseAllButtons() {}
    void setLeftStick(int8_t x, int8_t y) {}
    void setRightStick(int8_t x, int8_t y) {}
    void centerSticks() {}
    void setDPad(uint8_t direction) {}
    void setLeftTrigger(uint8_t value) {}
    void setRightTrigger(uint8_t value) {}
    void sendReport() {}
    String getStatus() { return "Gamepad not available (no native USB)"; }
    bool isInitialized() { return false; }
};

#endif

#endif // GAMEPAD_CONTROLLER_H
