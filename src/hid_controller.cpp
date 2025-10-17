#include "hid_controller.h"
#include "config.h"
#include <ArduinoJson.h>

#if HAVE_NATIVE_USB

HIDController::HIDController(USBHIDKeyboard* kb, USBHIDMouse* ms) 
    : keyboard(kb), mouse(ms), isInitialized(false) {}

HIDController::~HIDController() {}

bool HIDController::begin() {
    if (!keyboard || !mouse) {
        DEBUG_PRINTLN("HID devices not initialized");
        return false;
    }
    isInitialized = true;
    DEBUG_PRINTLN("HID Controller initialized");
    return true;
}

bool HIDController::typeText(const String& text) {
    if (!isReady()) return false;
    
    for (int i = 0; i < text.length(); i++) {
        char c = text.charAt(i);
        keyboard->write(c);
        delay(10); // Small delay between keystrokes
    }
    
    DEBUG_PRINTF("Typed text: %s\n", text.c_str());
    return true;
}

bool HIDController::pressKey(uint8_t key, uint8_t modifiers) {
    if (!isReady()) return false;
    
    keyboard->pressRaw(key);
    if (modifiers & KEY_CTRL) keyboard->press(KEY_LEFT_CTRL);
    if (modifiers & KEY_SHIFT) keyboard->press(KEY_LEFT_SHIFT);
    if (modifiers & KEY_ALT) keyboard->press(KEY_LEFT_ALT);
    if (modifiers & KEY_GUI) keyboard->press(KEY_LEFT_GUI);
    
    return true;
}

bool HIDController::releaseKey(uint8_t key, uint8_t modifiers) {
    if (!isReady()) return false;
    
    keyboard->releaseRaw(key);
    if (modifiers & KEY_CTRL) keyboard->release(KEY_LEFT_CTRL);
    if (modifiers & KEY_SHIFT) keyboard->release(KEY_LEFT_SHIFT);
    if (modifiers & KEY_ALT) keyboard->release(KEY_LEFT_ALT);
    if (modifiers & KEY_GUI) keyboard->release(KEY_LEFT_GUI);
    
    return true;
}

bool HIDController::sendKeyStroke(uint8_t key, uint8_t modifiers) {
    if (!isReady()) return false;
    
    pressKey(key, modifiers);
    delay(50);
    releaseKey(key, modifiers);
    
    return true;
}

bool HIDController::sendKeyStroke(const String& keyName, const String& modifiers) {
    if (!isReady()) return false;
    
    uint8_t modifierFlags = 0;
    if (modifiers.indexOf("ctrl") >= 0) modifierFlags |= KEY_CTRL;
    if (modifiers.indexOf("shift") >= 0) modifierFlags |= KEY_SHIFT;
    if (modifiers.indexOf("alt") >= 0) modifierFlags |= KEY_ALT;
    if (modifiers.indexOf("gui") >= 0 || modifiers.indexOf("cmd") >= 0 || modifiers.indexOf("win") >= 0) {
        modifierFlags |= KEY_GUI;
    }
    
    // Handle special keys
    uint8_t key = 0;
    if (keyName == "Enter" || keyName == "Return") {
        key = KEY_RETURN;
    } else if (keyName == "Escape" || keyName == "Esc") {
        key = KEY_ESC;
    } else if (keyName == "Tab") {
        key = KEY_TAB;
    } else if (keyName == "Space") {
        key = ' ';
    } else if (keyName == "Backspace") {
        key = KEY_BACKSPACE;
    } else if (keyName == "Delete" || keyName == "Del") {
        key = KEY_DELETE;
    } else if (keyName == "Up") {
        key = KEY_UP_ARROW;
    } else if (keyName == "Down") {
        key = KEY_DOWN_ARROW;
    } else if (keyName == "Left") {
        key = KEY_LEFT_ARROW;
    } else if (keyName == "Right") {
        key = KEY_RIGHT_ARROW;
    } else if (keyName.startsWith("F") && keyName.length() <= 3) {
        // Function keys F1-F12
        int fNum = keyName.substring(1).toInt();
        if (fNum >= 1 && fNum <= 12) {
            key = KEY_F1 + (fNum - 1);
        }
    } else if (keyName.length() == 1) {
        // Single character
        key = keyName.charAt(0);
    }
    
    if (key == 0) {
        DEBUG_PRINTF("Unknown key: %s\n", keyName.c_str());
        return false;
    }
    
    // Press modifiers first
    if (modifierFlags & KEY_CTRL) keyboard->press(KEY_LEFT_CTRL);
    if (modifierFlags & KEY_SHIFT) keyboard->press(KEY_LEFT_SHIFT);
    if (modifierFlags & KEY_ALT) keyboard->press(KEY_LEFT_ALT);
    if (modifierFlags & KEY_GUI) keyboard->press(KEY_LEFT_GUI);
    
    // Press and release the main key
    keyboard->press(key);
    delay(50);
    keyboard->release(key);
    
    // Release modifiers
    if (modifierFlags & KEY_GUI) keyboard->release(KEY_LEFT_GUI);
    if (modifierFlags & KEY_ALT) keyboard->release(KEY_LEFT_ALT);
    if (modifierFlags & KEY_SHIFT) keyboard->release(KEY_LEFT_SHIFT);
    if (modifierFlags & KEY_CTRL) keyboard->release(KEY_LEFT_CTRL);
    
    DEBUG_PRINTF("Sent keystroke: %s with modifiers: %s\n", keyName.c_str(), modifiers.c_str());
    return true;
}

bool HIDController::sendKeySequence(const String& sequence) {
    // Simple implementation - type each character
    return typeText(sequence);
}

bool HIDController::sendCtrlC() {
    return sendKeyStroke("c", "ctrl");
}

bool HIDController::sendCtrlV() {
    return sendKeyStroke("v", "ctrl");
}

bool HIDController::sendCtrlZ() {
    return sendKeyStroke("z", "ctrl");
}

bool HIDController::sendAltTab() {
    return sendKeyStroke("Tab", "alt");
}

bool HIDController::sendWinKey() {
    return sendKeyStroke("", "gui");
}

bool HIDController::moveMouse(int16_t x, int16_t y, bool relative) {
    if (!isReady()) return false;
    
    if (relative) {
        mouse->move(x, y);
    } else {
        // For absolute positioning, we'd need screen resolution
        // For now, treat as relative
        mouse->move(x, y);
    }
    
    DEBUG_PRINTF("Mouse moved: x=%d, y=%d, relative=%d\n", x, y, relative);
    return true;
}

bool HIDController::clickMouse(uint8_t button, uint16_t duration) {
    if (!isReady()) return false;
    
    mouse->press(button);
    delay(duration);
    mouse->release(button);
    
    DEBUG_PRINTF("Mouse clicked: button=%d, duration=%d\n", button, duration);
    return true;
}

bool HIDController::doubleClickMouse(uint8_t button) {
    if (!isReady()) return false;
    
    clickMouse(button, 50);
    delay(50);
    clickMouse(button, 50);
    
    DEBUG_PRINTF("Mouse double-clicked: button=%d\n", button);
    return true;
}

bool HIDController::pressMouse(uint8_t button) {
    if (!isReady()) return false;
    
    mouse->press(button);
    return true;
}

bool HIDController::releaseMouse(uint8_t button) {
    if (!isReady()) return false;
    
    mouse->release(button);
    return true;
}

bool HIDController::scrollMouse(int8_t scroll) {
    if (!isReady()) return false;
    
    mouse->move(0, 0, scroll);
    
    DEBUG_PRINTF("Mouse scrolled: %d\n", scroll);
    return true;
}

bool HIDController::isReady() {
    return isInitialized && keyboard && mouse;
}

void HIDController::reset() {
    if (keyboard) {
        keyboard->releaseAll();
    }
    if (mouse) {
        mouse->release(MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE);
    }
}

String HIDController::getStatus() {
    DynamicJsonDocument status(256);
    status["initialized"] = isInitialized;
    status["keyboard_ready"] = keyboard != nullptr;
    status["mouse_ready"] = mouse != nullptr;
    status["overall_ready"] = isReady();
    
    String statusStr;
    serializeJson(status, statusStr);
    return statusStr;
}

uint8_t HIDController::mapSpecialKey(const String& keyName) {
    // This is handled in sendKeyStroke(String, String)
    return 0;
}

uint8_t HIDController::mapMouseButton(const String& buttonName) {
    if (buttonName == "left") return MOUSE_LEFT;
    if (buttonName == "right") return MOUSE_RIGHT;
    if (buttonName == "middle") return MOUSE_MIDDLE;
    return MOUSE_LEFT; // Default
}

#endif // HAVE_NATIVE_USB
