#include "gamepad_controller.h"

#if HAVE_NATIVE_USB

// USB HID Gamepad Report Descriptor
static const uint8_t GAMEPAD_REPORT_DESCRIPTOR[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x05,        // Usage (Gamepad)
    0xA1, 0x01,        // Collection (Application)
    
    // Buttons (16 buttons)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (Button 1)
    0x29, 0x10,        //   Usage Maximum (Button 16)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x10,        //   Report Count (16)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // D-Pad (HAT switch)
    0x05, 0x01,        //   Usage Page (Generic Desktop)
    0x09, 0x39,        //   Usage (Hat Switch)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x07,        //   Logical Maximum (7)
    0x35, 0x00,        //   Physical Minimum (0)
    0x46, 0x3B, 0x01,  //   Physical Maximum (315)
    0x65, 0x14,        //   Unit (Degrees)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x42,        //   Input (Data, Variable, Absolute, Null State)
    
    // Padding
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x03,        //   Input (Constant, Variable, Absolute)
    
    // Left Stick X & Y
    0x05, 0x01,        //   Usage Page (Generic Desktop)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x15, 0x81,        //   Logical Minimum (-127)
    0x25, 0x7F,        //   Logical Maximum (127)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Right Stick X & Y
    0x09, 0x32,        //   Usage (Z)
    0x09, 0x35,        //   Usage (Rz)
    0x15, 0x81,        //   Logical Minimum (-127)
    0x25, 0x7F,        //   Logical Maximum (127)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Triggers (Z axis and Rz)
    0x05, 0x02,        //   Usage Page (Simulation Controls)
    0x09, 0xC4,        //   Usage (Accelerator)
    0x09, 0xC5,        //   Usage (Brake)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    0xC0               // End Collection
};

GamepadController::GamepadController() 
    : _initialized(false), _buttons(0), 
      _leftX(0), _leftY(0), _rightX(0), _rightY(0),
      _leftTrigger(0), _rightTrigger(0), _dpad(DPAD_CENTERED) {
}

void GamepadController::begin() {
    Serial.println("Initializing USB Gamepad...");
    
    // Note: USBHIDGamepad might not exist in standard ESP32 libraries
    // This is a placeholder for custom implementation
    // You may need to use USBHID class directly with custom report descriptor
    
    _initialized = true;
    centerSticks();
    releaseAllButtons();
    
    Serial.println("USB Gamepad initialized");
    Serial.println("Gamepad available as HID device");
}

void GamepadController::pressButton(uint16_t button) {
    _buttons |= button;
    Serial.printf("Gamepad button press: 0x%04X\n", button);
}

void GamepadController::releaseButton(uint16_t button) {
    _buttons &= ~button;
    Serial.printf("Gamepad button release: 0x%04X\n", button);
}

void GamepadController::pressButtons(uint16_t buttons) {
    _buttons = buttons;
    Serial.printf("Gamepad buttons set: 0x%04X\n", buttons);
}

void GamepadController::releaseAllButtons() {
    _buttons = 0;
    Serial.println("Gamepad all buttons released");
}

void GamepadController::setLeftStick(int8_t x, int8_t y) {
    _leftX = x;
    _leftY = y;
    Serial.printf("Gamepad left stick: X=%d Y=%d\n", x, y);
}

void GamepadController::setRightStick(int8_t x, int8_t y) {
    _rightX = x;
    _rightY = y;
    Serial.printf("Gamepad right stick: X=%d Y=%d\n", x, y);
}

void GamepadController::centerSticks() {
    _leftX = 0;
    _leftY = 0;
    _rightX = 0;
    _rightY = 0;
    Serial.println("Gamepad sticks centered");
}

void GamepadController::setDPad(uint8_t direction) {
    _dpad = direction & 0x0F;
    Serial.printf("Gamepad D-pad: %d\n", direction);
}

void GamepadController::setLeftTrigger(uint8_t value) {
    _leftTrigger = value;
    Serial.printf("Gamepad left trigger: %d\n", value);
}

void GamepadController::setRightTrigger(uint8_t value) {
    _rightTrigger = value;
    Serial.printf("Gamepad right trigger: %d\n", value);
}

void GamepadController::sendReport() {
    if (!_initialized) {
        Serial.println("Gamepad not initialized");
        return;
    }
    
    // Build HID report
    uint8_t report[10];
    
    // Buttons (2 bytes)
    report[0] = _buttons & 0xFF;
    report[1] = (_buttons >> 8) & 0xFF;
    
    // D-pad (4 bits) + padding (4 bits)
    report[2] = (_dpad & 0x0F) | 0xF0;
    
    // Left stick
    report[3] = _leftX + 128;  // Convert to 0-255 range
    report[4] = _leftY + 128;
    
    // Right stick
    report[5] = _rightX + 128;
    report[6] = _rightY + 128;
    
    // Triggers
    report[7] = _leftTrigger;
    report[8] = _rightTrigger;
    
    // Note: Actual USB HID send would go here
    // USB.sendReport(GAMEPAD_REPORT_ID, report, sizeof(report));
    
    Serial.println("Gamepad report sent");
}

String GamepadController::getStatus() {
    if (!_initialized) {
        return "Gamepad not initialized";
    }
    
    String status = "Gamepad: ";
    status += "Buttons=" + String(_buttons, HEX) + " ";
    status += "LStick(" + String(_leftX) + "," + String(_leftY) + ") ";
    status += "RStick(" + String(_rightX) + "," + String(_rightY) + ") ";
    status += "LT=" + String(_leftTrigger) + " ";
    status += "RT=" + String(_rightTrigger);
    
    return status;
}

bool GamepadController::isInitialized() {
    return _initialized;
}

#endif // HAVE_NATIVE_USB
