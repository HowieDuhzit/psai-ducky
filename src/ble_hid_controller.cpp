#include "ble_hid_controller.h"

#if !HAVE_NATIVE_USB

BLEHIDController::BLEHIDController(const char* deviceName) 
    : _deviceName(deviceName), _initialized(false) {
    _keyboard = new BleKeyboard(deviceName, "HowieDuhzit", 100);
    _mouse = new BleMouse(deviceName, "HowieDuhzit", 100);
}

void BLEHIDController::begin() {
    Serial.println("Initializing BLE HID...");
    
    // Start BLE keyboard
    _keyboard->begin();
    Serial.println("BLE Keyboard started");
    
    // Start BLE mouse
    _mouse->begin();
    Serial.println("BLE Mouse started");
    
    _initialized = true;
    Serial.printf("BLE HID initialized as '%s'\n", _deviceName.c_str());
    Serial.println("Waiting for BLE connection...");
}

bool BLEHIDController::isConnected() {
    return _keyboard->isConnected() || _mouse->isConnected();
}

void BLEHIDController::typeText(const String& text) {
    if (!_keyboard->isConnected()) {
        Serial.println("BLE Keyboard not connected");
        return;
    }
    
    _keyboard->print(text);
    Serial.printf("BLE typed: %s\n", text.c_str());
}

void BLEHIDController::pressKey(uint8_t key) {
    if (!_keyboard->isConnected()) {
        Serial.println("BLE Keyboard not connected");
        return;
    }
    
    _keyboard->press(key);
    Serial.printf("BLE key press: 0x%02X\n", key);
}

void BLEHIDController::releaseKey(uint8_t key) {
    if (!_keyboard->isConnected()) {
        Serial.println("BLE Keyboard not connected");
        return;
    }
    
    _keyboard->release(key);
    Serial.printf("BLE key release: 0x%02X\n", key);
}

void BLEHIDController::pressKeys(uint8_t modifiers, uint8_t key) {
    if (!_keyboard->isConnected()) {
        Serial.println("BLE Keyboard not connected");
        return;
    }
    
    // Press modifiers
    if (modifiers & 0x01) _keyboard->press(KEY_LEFT_CTRL);
    if (modifiers & 0x02) _keyboard->press(KEY_LEFT_SHIFT);
    if (modifiers & 0x04) _keyboard->press(KEY_LEFT_ALT);
    if (modifiers & 0x08) _keyboard->press(KEY_LEFT_GUI);
    if (modifiers & 0x10) _keyboard->press(KEY_RIGHT_CTRL);
    if (modifiers & 0x20) _keyboard->press(KEY_RIGHT_SHIFT);
    if (modifiers & 0x40) _keyboard->press(KEY_RIGHT_ALT);
    if (modifiers & 0x80) _keyboard->press(KEY_RIGHT_GUI);
    
    // Press key
    if (key != 0) {
        _keyboard->press(key);
    }
    
    delay(10);
    
    // Release all
    _keyboard->releaseAll();
    
    Serial.printf("BLE key combo: mod=0x%02X key=0x%02X\n", modifiers, key);
}

void BLEHIDController::mouseMove(int x, int y) {
    if (!_mouse->isConnected()) {
        Serial.println("BLE Mouse not connected");
        return;
    }
    
    _mouse->move(x, y);
    Serial.printf("BLE mouse move: x=%d y=%d\n", x, y);
}

void BLEHIDController::mouseClick(uint8_t button) {
    if (!_mouse->isConnected()) {
        Serial.println("BLE Mouse not connected");
        return;
    }
    
    _mouse->click(button);
    Serial.printf("BLE mouse click: button=%d\n", button);
}

void BLEHIDController::mouseScroll(int amount) {
    if (!_mouse->isConnected()) {
        Serial.println("BLE Mouse not connected");
        return;
    }
    
    _mouse->move(0, 0, amount);
    Serial.printf("BLE mouse scroll: %d\n", amount);
}

String BLEHIDController::getStatus() {
    if (!_initialized) {
        return "BLE HID not initialized";
    }
    
    String status = "BLE HID: ";
    if (_keyboard->isConnected() && _mouse->isConnected()) {
        status += "Keyboard & Mouse connected";
    } else if (_keyboard->isConnected()) {
        status += "Keyboard connected";
    } else if (_mouse->isConnected()) {
        status += "Mouse connected";
    } else {
        status += "Waiting for connection...";
    }
    
    return status;
}

#endif // !HAVE_NATIVE_USB
