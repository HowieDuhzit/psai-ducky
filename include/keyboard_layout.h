#ifndef KEYBOARD_LAYOUT_H
#define KEYBOARD_LAYOUT_H

#include <Arduino.h>
#include <map>

// Supported keyboard layouts
enum KeyboardLayout {
    LAYOUT_US,      // US English (QWERTY)
    LAYOUT_UK,      // UK English (QWERTY)
    LAYOUT_DE,      // German (QWERTZ)
    LAYOUT_FR,      // French (AZERTY)
    LAYOUT_ES,      // Spanish
    LAYOUT_IT,      // Italian
    LAYOUT_PT,      // Portuguese
    LAYOUT_SE,      // Swedish
    LAYOUT_NO,      // Norwegian
    LAYOUT_DK       // Danish
};

// Keycode structure
struct KeyCode {
    uint8_t scancode;
    uint8_t modifiers;  // 0x01=Ctrl, 0x02=Shift, 0x04=Alt, 0x08=GUI
};

class KeyboardLayoutManager {
public:
    KeyboardLayoutManager();
    void begin();
    
    // Layout selection
    void setLayout(KeyboardLayout layout);
    KeyboardLayout getLayout();
    String getLayoutName();
    String getLayoutName(KeyboardLayout layout);
    
    // Character to keycode mapping
    KeyCode charToKeyCode(char ch);
    KeyCode stringToKeyCode(const String& str, size_t index);
    
    // Type string with current layout
    std::vector<KeyCode> convertString(const String& text);
    
    // Get available layouts
    std::vector<KeyboardLayout> getAvailableLayouts();
    
private:
    KeyboardLayout _currentLayout;
    bool _initialized;
    
    // Layout-specific mapping functions
    KeyCode getUSKeyCode(char ch);
    KeyCode getUKKeyCode(char ch);
    KeyCode getDEKeyCode(char ch);
    KeyCode getFRKeyCode(char ch);
    KeyCode getESKeyCode(char ch);
    KeyCode getITKeyCode(char ch);
    
    // Helper to check if character needs shift
    bool needsShift(char ch, KeyboardLayout layout);
};

#endif // KEYBOARD_LAYOUT_H
