#ifndef CONFIG_H
#define CONFIG_H

// MCP Server Configuration
#define MCP_SERVER_PORT 8080
#define MCP_SERVER_VERSION "1.0.0"
#define MCP_SERVER_NAME "ESP32-HID-Controller"

// WiFi Configuration - Leave empty to start in AP mode
// Override via config/wifi_credentials.h (ignored by git)
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define WIFI_TIMEOUT 10000               // 10 seconds
#define AP_SSID "ESP32-MCP-Server"
#define AP_PASSWORD "esp32mcp123"

#ifdef __has_include
#  if __has_include("config/wifi_credentials.h")
#    include "config/wifi_credentials.h"
#  endif
#endif

// HID Configuration
#define KEYBOARD_LAYOUT KEYBOARD_LAYOUT_US
#define MOUSE_SENSITIVITY 1.0
#define MAX_KEY_SEQUENCE_LENGTH 256

// Security Configuration
#define ENABLE_AUTHENTICATION false
#define API_KEY ""      
#define MAX_CLIENTS 5
#define CONNECTION_TIMEOUT 30000         // 30 seconds

// Debug Configuration - controlled via build_flags (-DDEBUG_MCP_SERVER)
#ifdef DEBUG_MCP_SERVER
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(x, ...) Serial.printf(x, __VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(x, ...)
#endif

// MCP Protocol Constants
#define MCP_PROTOCOL_VERSION "2024-11-05"
#define MCP_IMPLEMENTATION_NAME "esp32-hid-mcp-server"
#define MCP_IMPLEMENTATION_VERSION MCP_SERVER_VERSION

// Available MCP Tools
#define TOOL_KEYBOARD_TYPE "keyboard_type"
#define TOOL_KEYBOARD_KEY "keyboard_key"
#define TOOL_KEYBOARD_SHORTCUT "keyboard_shortcut"
#define TOOL_MOUSE_MOVE "mouse_move"
#define TOOL_MOUSE_CLICK "mouse_click"
#define TOOL_MOUSE_SCROLL "mouse_scroll"
#define TOOL_SYSTEM_STATUS "system_status"

#endif // CONFIG_H
