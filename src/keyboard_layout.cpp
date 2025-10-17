#include "keyboard_layout.h"

// USB HID Keyboard Scancodes
#define KEY_A 0x04
#define KEY_B 0x05
#define KEY_C 0x06
#define KEY_D 0x07
#define KEY_E 0x08
#define KEY_F 0x09
#define KEY_G 0x0A
#define KEY_H 0x0B
#define KEY_I 0x0C
#define KEY_J 0x0D
#define KEY_K 0x0E
#define KEY_L 0x0F
#define KEY_M 0x10
#define KEY_N 0x11
#define KEY_O 0x12
#define KEY_P 0x13
#define KEY_Q 0x14
#define KEY_R 0x15
#define KEY_S 0x16
#define KEY_T 0x17
#define KEY_U 0x18
#define KEY_V 0x19
#define KEY_W 0x1A
#define KEY_X 0x1B
#define KEY_Y 0x1C
#define KEY_Z 0x1D

#define KEY_1 0x1E
#define KEY_2 0x1F
#define KEY_3 0x20
#define KEY_4 0x21
#define KEY_5 0x22
#define KEY_6 0x23
#define KEY_7 0x24
#define KEY_8 0x25
#define KEY_9 0x26
#define KEY_0 0x27

#define KEY_ENTER 0x28
#define KEY_ESC 0x29
#define KEY_BACKSPACE 0x2A
#define KEY_TAB 0x2B
#define KEY_SPACE 0x2C
#define KEY_MINUS 0x2D
#define KEY_EQUAL 0x2E
#define KEY_LEFTBRACE 0x2F
#define KEY_RIGHTBRACE 0x30
#define KEY_BACKSLASH 0x31
#define KEY_SEMICOLON 0x33
#define KEY_APOSTROPHE 0x34
#define KEY_GRAVE 0x35
#define KEY_COMMA 0x36
#define KEY_DOT 0x37
#define KEY_SLASH 0x38

// Modifiers
#define MOD_NONE 0x00
#define MOD_SHIFT 0x02

KeyboardLayoutManager::KeyboardLayoutManager() 
    : _currentLayout(LAYOUT_US), _initialized(false) {
}

void KeyboardLayoutManager::begin() {
    _initialized = true;
    Serial.println("Keyboard Layout Manager initialized");
    Serial.printf("Default layout: %s\n", getLayoutName().c_str());
}

void KeyboardLayoutManager::setLayout(KeyboardLayout layout) {
    _currentLayout = layout;
    Serial.printf("Keyboard layout changed to: %s\n", getLayoutName().c_str());
}

KeyboardLayout KeyboardLayoutManager::getLayout() {
    return _currentLayout;
}

String KeyboardLayoutManager::getLayoutName() {
    return getLayoutName(_currentLayout);
}

String KeyboardLayoutManager::getLayoutName(KeyboardLayout layout) {
    switch (layout) {
        case LAYOUT_US: return "US (QWERTY)";
        case LAYOUT_UK: return "UK (QWERTY)";
        case LAYOUT_DE: return "German (QWERTZ)";
        case LAYOUT_FR: return "French (AZERTY)";
        case LAYOUT_ES: return "Spanish";
        case LAYOUT_IT: return "Italian";
        case LAYOUT_PT: return "Portuguese";
        case LAYOUT_SE: return "Swedish";
        case LAYOUT_NO: return "Norwegian";
        case LAYOUT_DK: return "Danish";
        default: return "Unknown";
    }
}

KeyCode KeyboardLayoutManager::charToKeyCode(char ch) {
    switch (_currentLayout) {
        case LAYOUT_US: return getUSKeyCode(ch);
        case LAYOUT_UK: return getUKKeyCode(ch);
        case LAYOUT_DE: return getDEKeyCode(ch);
        case LAYOUT_FR: return getFRKeyCode(ch);
        case LAYOUT_ES: return getESKeyCode(ch);
        case LAYOUT_IT: return getITKeyCode(ch);
        default: return getUSKeyCode(ch);  // Fallback to US
    }
}

KeyCode KeyboardLayoutManager::stringToKeyCode(const String& str, size_t index) {
    if (index >= str.length()) {
        return {0, 0};
    }
    return charToKeyCode(str[index]);
}

std::vector<KeyCode> KeyboardLayoutManager::convertString(const String& text) {
    std::vector<KeyCode> keycodes;
    
    for (size_t i = 0; i < text.length(); i++) {
        KeyCode kc = charToKeyCode(text[i]);
        if (kc.scancode != 0) {
            keycodes.push_back(kc);
        }
    }
    
    return keycodes;
}

std::vector<KeyboardLayout> KeyboardLayoutManager::getAvailableLayouts() {
    return {
        LAYOUT_US, LAYOUT_UK, LAYOUT_DE, LAYOUT_FR,
        LAYOUT_ES, LAYOUT_IT, LAYOUT_PT, LAYOUT_SE,
        LAYOUT_NO, LAYOUT_DK
    };
}

// US Layout (QWERTY)
KeyCode KeyboardLayoutManager::getUSKeyCode(char ch) {
    KeyCode kc = {0, MOD_NONE};
    
    // Lowercase letters
    if (ch >= 'a' && ch <= 'z') {
        kc.scancode = KEY_A + (ch - 'a');
        return kc;
    }
    
    // Uppercase letters
    if (ch >= 'A' && ch <= 'Z') {
        kc.scancode = KEY_A + (ch - 'A');
        kc.modifiers = MOD_SHIFT;
        return kc;
    }
    
    // Numbers
    if (ch >= '1' && ch <= '9') {
        kc.scancode = KEY_1 + (ch - '1');
        return kc;
    }
    
    // Special characters
    switch (ch) {
        case '0': kc.scancode = KEY_0; break;
        case ' ': kc.scancode = KEY_SPACE; break;
        case '\n': kc.scancode = KEY_ENTER; break;
        case '\t': kc.scancode = KEY_TAB; break;
        
        // Unshifted symbols
        case '-': kc.scancode = KEY_MINUS; break;
        case '=': kc.scancode = KEY_EQUAL; break;
        case '[': kc.scancode = KEY_LEFTBRACE; break;
        case ']': kc.scancode = KEY_RIGHTBRACE; break;
        case '\\': kc.scancode = KEY_BACKSLASH; break;
        case ';': kc.scancode = KEY_SEMICOLON; break;
        case '\'': kc.scancode = KEY_APOSTROPHE; break;
        case '`': kc.scancode = KEY_GRAVE; break;
        case ',': kc.scancode = KEY_COMMA; break;
        case '.': kc.scancode = KEY_DOT; break;
        case '/': kc.scancode = KEY_SLASH; break;
        
        // Shifted symbols
        case '!': kc.scancode = KEY_1; kc.modifiers = MOD_SHIFT; break;
        case '@': kc.scancode = KEY_2; kc.modifiers = MOD_SHIFT; break;
        case '#': kc.scancode = KEY_3; kc.modifiers = MOD_SHIFT; break;
        case '$': kc.scancode = KEY_4; kc.modifiers = MOD_SHIFT; break;
        case '%': kc.scancode = KEY_5; kc.modifiers = MOD_SHIFT; break;
        case '^': kc.scancode = KEY_6; kc.modifiers = MOD_SHIFT; break;
        case '&': kc.scancode = KEY_7; kc.modifiers = MOD_SHIFT; break;
        case '*': kc.scancode = KEY_8; kc.modifiers = MOD_SHIFT; break;
        case '(': kc.scancode = KEY_9; kc.modifiers = MOD_SHIFT; break;
        case ')': kc.scancode = KEY_0; kc.modifiers = MOD_SHIFT; break;
        case '_': kc.scancode = KEY_MINUS; kc.modifiers = MOD_SHIFT; break;
        case '+': kc.scancode = KEY_EQUAL; kc.modifiers = MOD_SHIFT; break;
        case '{': kc.scancode = KEY_LEFTBRACE; kc.modifiers = MOD_SHIFT; break;
        case '}': kc.scancode = KEY_RIGHTBRACE; kc.modifiers = MOD_SHIFT; break;
        case '|': kc.scancode = KEY_BACKSLASH; kc.modifiers = MOD_SHIFT; break;
        case ':': kc.scancode = KEY_SEMICOLON; kc.modifiers = MOD_SHIFT; break;
        case '"': kc.scancode = KEY_APOSTROPHE; kc.modifiers = MOD_SHIFT; break;
        case '~': kc.scancode = KEY_GRAVE; kc.modifiers = MOD_SHIFT; break;
        case '<': kc.scancode = KEY_COMMA; kc.modifiers = MOD_SHIFT; break;
        case '>': kc.scancode = KEY_DOT; kc.modifiers = MOD_SHIFT; break;
        case '?': kc.scancode = KEY_SLASH; kc.modifiers = MOD_SHIFT; break;
    }
    
    return kc;
}

// UK Layout (QWERTY with differences)
KeyCode KeyboardLayoutManager::getUKKeyCode(char ch) {
    KeyCode kc = getUSKeyCode(ch);  // Start with US layout
    
    // UK-specific character mappings
    switch (ch) {
        case '"': kc.scancode = KEY_2; kc.modifiers = MOD_SHIFT; break;
        case '#': kc.scancode = KEY_3; kc.modifiers = MOD_NONE; break;  // UK: # is on 3
        case '£': kc.scancode = KEY_3; kc.modifiers = MOD_SHIFT; break;
        case '@': kc.scancode = KEY_APOSTROPHE; kc.modifiers = MOD_SHIFT; break;
        case '~': kc.scancode = KEY_GRAVE; kc.modifiers = MOD_SHIFT; break;
        case '|': kc.scancode = KEY_BACKSLASH; kc.modifiers = MOD_SHIFT; break;
    }
    
    return kc;
}

// German Layout (QWERTZ)
KeyCode KeyboardLayoutManager::getDEKeyCode(char ch) {
    KeyCode kc = {0, MOD_NONE};
    
    // Lowercase letters - note Y and Z are swapped in QWERTZ
    if (ch >= 'a' && ch <= 'z') {
        if (ch == 'y') {
            kc.scancode = KEY_Z;  // Y is where Z would be
        } else if (ch == 'z') {
            kc.scancode = KEY_Y;  // Z is where Y would be
        } else {
            kc.scancode = KEY_A + (ch - 'a');
        }
        return kc;
    }
    
    // Uppercase letters
    if (ch >= 'A' && ch <= 'Z') {
        if (ch == 'Y') {
            kc.scancode = KEY_Z;
            kc.modifiers = MOD_SHIFT;
        } else if (ch == 'Z') {
            kc.scancode = KEY_Y;
            kc.modifiers = MOD_SHIFT;
        } else {
            kc.scancode = KEY_A + (ch - 'A');
            kc.modifiers = MOD_SHIFT;
        }
        return kc;
    }
    
    // Numbers and common symbols
    if (ch >= '0' && ch <= '9') {
        kc.scancode = KEY_0 + ((ch - '0') % 10);
        return kc;
    }
    
    // German-specific characters
    switch (ch) {
        case 'ä': kc.scancode = KEY_APOSTROPHE; break;
        case 'Ä': kc.scancode = KEY_APOSTROPHE; kc.modifiers = MOD_SHIFT; break;
        case 'ö': kc.scancode = KEY_SEMICOLON; break;
        case 'Ö': kc.scancode = KEY_SEMICOLON; kc.modifiers = MOD_SHIFT; break;
        case 'ü': kc.scancode = KEY_LEFTBRACE; break;
        case 'Ü': kc.scancode = KEY_LEFTBRACE; kc.modifiers = MOD_SHIFT; break;
        case 'ß': kc.scancode = KEY_MINUS; break;
        
        // Common symbols
        case ' ': kc.scancode = KEY_SPACE; break;
        case '\n': kc.scancode = KEY_ENTER; break;
        case '\t': kc.scancode = KEY_TAB; break;
        case '.': kc.scancode = KEY_DOT; break;
        case ',': kc.scancode = KEY_COMMA; break;
        case '-': kc.scancode = KEY_SLASH; break;
        case '+': kc.scancode = KEY_RIGHTBRACE; break;
        case '#': kc.scancode = KEY_BACKSLASH; break;
    }
    
    return kc;
}

// French Layout (AZERTY)
KeyCode KeyboardLayoutManager::getFRKeyCode(char ch) {
    KeyCode kc = {0, MOD_NONE};
    
    // AZERTY has A-Z in different positions
    const uint8_t azerty_map[] = {
        KEY_Q,  // a -> q position
        KEY_B,  // b
        KEY_C,  // c
        KEY_D,  // d
        KEY_E,  // e
        KEY_F,  // f
        KEY_G,  // g
        KEY_H,  // h
        KEY_I,  // i
        KEY_J,  // j
        KEY_K,  // k
        KEY_L,  // l
        KEY_SEMICOLON,  // m
        KEY_N,  // n
        KEY_O,  // o
        KEY_P,  // p
        KEY_A,  // q -> a position
        KEY_R,  // r
        KEY_S,  // s
        KEY_T,  // t
        KEY_U,  // u
        KEY_V,  // v
        KEY_Z,  // w -> z position
        KEY_X,  // x
        KEY_Y,  // y
        KEY_W   // z -> w position
    };
    
    // Lowercase letters
    if (ch >= 'a' && ch <= 'z') {
        kc.scancode = azerty_map[ch - 'a'];
        return kc;
    }
    
    // Uppercase letters
    if (ch >= 'A' && ch <= 'Z') {
        kc.scancode = azerty_map[ch - 'A'];
        kc.modifiers = MOD_SHIFT;
        return kc;
    }
    
    // Numbers require shift in AZERTY
    if (ch >= '1' && ch <= '9') {
        kc.scancode = KEY_1 + (ch - '1');
        kc.modifiers = MOD_SHIFT;
        return kc;
    }
    
    // Common symbols
    switch (ch) {
        case '0': kc.scancode = KEY_0; kc.modifiers = MOD_SHIFT; break;
        case ' ': kc.scancode = KEY_SPACE; break;
        case '\n': kc.scancode = KEY_ENTER; break;
        case '\t': kc.scancode = KEY_TAB; break;
        case '.': kc.scancode = KEY_DOT; kc.modifiers = MOD_SHIFT; break;
        case ',': kc.scancode = KEY_M; break;
        case ';': kc.scancode = KEY_COMMA; break;
        case ':': kc.scancode = KEY_DOT; break;
        
        // French accented characters
        case 'é': kc.scancode = KEY_2; break;
        case 'è': kc.scancode = KEY_7; break;
        case 'ç': kc.scancode = KEY_9; break;
        case 'à': kc.scancode = KEY_0; break;
    }
    
    return kc;
}

// Spanish Layout
KeyCode KeyboardLayoutManager::getESKeyCode(char ch) {
    KeyCode kc = getUSKeyCode(ch);  // Start with US layout base
    
    // Spanish-specific characters
    switch (ch) {
        case 'ñ': kc.scancode = KEY_SEMICOLON; break;
        case 'Ñ': kc.scancode = KEY_SEMICOLON; kc.modifiers = MOD_SHIFT; break;
        case 'á': kc.scancode = KEY_APOSTROPHE; break;
        case 'é': kc.scancode = KEY_APOSTROPHE; kc.modifiers = MOD_SHIFT; break;
        case 'í': kc.scancode = KEY_APOSTROPHE; break;
        case 'ó': kc.scancode = KEY_APOSTROPHE; kc.modifiers = MOD_SHIFT; break;
        case 'ú': kc.scancode = KEY_APOSTROPHE; break;
        case '¿': kc.scancode = KEY_SLASH; kc.modifiers = MOD_SHIFT; break;
        case '¡': kc.scancode = KEY_1; kc.modifiers = MOD_SHIFT; break;
    }
    
    return kc;
}

// Italian Layout
KeyCode KeyboardLayoutManager::getITKeyCode(char ch) {
    KeyCode kc = getUSKeyCode(ch);  // Start with US layout base
    
    // Italian-specific characters
    switch (ch) {
        case 'à': kc.scancode = KEY_APOSTROPHE; break;
        case 'è': kc.scancode = KEY_LEFTBRACE; break;
        case 'é': kc.scancode = KEY_LEFTBRACE; kc.modifiers = MOD_SHIFT; break;
        case 'ì': kc.scancode = KEY_EQUAL; break;
        case 'ò': kc.scancode = KEY_SEMICOLON; break;
        case 'ù': kc.scancode = KEY_RIGHTBRACE; break;
    }
    
    return kc;
}

bool KeyboardLayoutManager::needsShift(char ch, KeyboardLayout layout) {
    // Check if character needs shift modifier
    if (ch >= 'A' && ch <= 'Z') return true;
    
    const char* shifted = "!@#$%^&*()_+{}|:\"<>?~";
    return (strchr(shifted, ch) != nullptr);
}
