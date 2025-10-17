#include "mcp_server.h"
#include "config.h"

// Static instance for callback
MCPServer* MCPServer::instance = nullptr;

MCPServer::MCPServer(WebSocketsServer* ws) : webSocket(ws), hidController(nullptr), isInitialized(false) {
    instance = this;
}

MCPServer::~MCPServer() {
    instance = nullptr;
}

bool MCPServer::begin() {
    if (!webSocket) {
        DEBUG_PRINTLN("WebSocket server not initialized");
        return false;
    }
    
    webSocket->begin();
    webSocket->onEvent(webSocketEventWrapper);
    webSocket->enableHeartbeat(15000, 3000, 2);
    
    isInitialized = true;
    DEBUG_PRINTLN("MCP Server started");
    return true;
}

void MCPServer::loop() {
    if (isInitialized) {
        webSocket->loop();
    }
}

void MCPServer::setHIDController(HIDController* controller) {
    hidController = controller;
}

void MCPServer::webSocketEventWrapper(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (instance) {
        instance->handleWebSocketEvent(num, type, payload, length);
    }
}

void MCPServer::handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            DEBUG_PRINTF("Client %d disconnected\n", num);
            break;
            
        case WStype_CONNECTED:
            DEBUG_PRINTF("Client %d connected from %s\n", num, webSocket->remoteIP(num).toString().c_str());
            break;
            
        case WStype_TEXT:
            DEBUG_PRINTF("Received message from client %d: %s\n", num, (char*)payload);
            handleMCPMessage(num, String((char*)payload));
            break;
            
        case WStype_ERROR:
            DEBUG_PRINTF("WebSocket error on client %d\n", num);
            break;
            
        default:
            break;
    }
}

void MCPServer::handleMCPMessage(uint8_t clientId, const String& message) {
    DynamicJsonDocument request(JSON_DOC_SIZE);
    DeserializationError error = deserializeJson(request, message);
    
    if (error) {
        DEBUG_PRINTF("JSON parsing failed: %s\n", error.c_str());
        sendMCPError(clientId, 0, "Invalid JSON");
        return;
    }
    
    String method = request["method"];
    int requestId = request["id"] | 0;
    
    if (method == "initialize") {
        handleInitialize(clientId, request);
    } else if (method == "tools/list") {
        handleListTools(clientId, request);
    } else if (method == "tools/call") {
        handleCallTool(clientId, request);
    } else {
        sendMCPError(clientId, requestId, "Unknown method: " + method);
    }
}

void MCPServer::sendMCPResponse(uint8_t clientId, const DynamicJsonDocument& response) {
    String responseStr;
    serializeJson(response, responseStr);
    webSocket->sendTXT(clientId, responseStr);
    DEBUG_PRINTF("Sent response to client %d: %s\n", clientId, responseStr.c_str());
}

void MCPServer::sendMCPError(uint8_t clientId, int requestId, const String& error) {
    DynamicJsonDocument response(512);
    response["jsonrpc"] = "2.0";
    response["id"] = requestId;
    response["error"]["code"] = -32000;
    response["error"]["message"] = error;
    sendMCPResponse(clientId, response);
}

void MCPServer::handleInitialize(uint8_t clientId, const DynamicJsonDocument& request) {
    DynamicJsonDocument response(1024);
    response["jsonrpc"] = "2.0";
    response["id"] = request["id"];
    
    JsonObject result = response.createNestedObject("result");
    result["protocolVersion"] = MCP_PROTOCOL_VERSION;
    result["serverInfo"]["name"] = MCP_IMPLEMENTATION_NAME;
    result["serverInfo"]["version"] = MCP_IMPLEMENTATION_VERSION;
    
    JsonObject capabilities = result.createNestedObject("capabilities");
    capabilities["tools"] = true;
    
    sendMCPResponse(clientId, response);
}

void MCPServer::handleListTools(uint8_t clientId, const DynamicJsonDocument& request) {
    DynamicJsonDocument response(JSON_DOC_SIZE);
    response["jsonrpc"] = "2.0";
    response["id"] = request["id"];

    JsonObject result = response.createNestedObject("result");
    JsonArray tools = result.createNestedArray("tools");

    // Keyboard Type Tool
    JsonObject keyboardType = tools.createNestedObject();
    keyboardType["name"] = TOOL_KEYBOARD_TYPE;
    keyboardType["description"] = "Type text using the keyboard";
    JsonObject keyboardTypeSchema = keyboardType.createNestedObject("inputSchema");
    keyboardTypeSchema["type"] = "object";
    JsonObject keyboardTypeProps = keyboardTypeSchema.createNestedObject("properties");
    JsonObject textProp = keyboardTypeProps.createNestedObject("text");
    textProp["type"] = "string";
    textProp["description"] = "Text to type";
    JsonArray keyboardTypeRequired = keyboardTypeSchema.createNestedArray("required");
    keyboardTypeRequired.add("text");
    
    // Keyboard Key Tool
    JsonObject keyboardKey = tools.createNestedObject();
    keyboardKey["name"] = TOOL_KEYBOARD_KEY;
    keyboardKey["description"] = "Press a specific key or key combination";
    JsonObject keyboardKeySchema = keyboardKey.createNestedObject("inputSchema");
    keyboardKeySchema["type"] = "object";
    JsonObject keyboardKeyProps = keyboardKeySchema.createNestedObject("properties");
    JsonObject keyProp = keyboardKeyProps.createNestedObject("key");
    keyProp["type"] = "string";
    keyProp["description"] = "Key to press (e.g., 'a', 'Enter', 'F1')";
    JsonObject modifiersProp = keyboardKeyProps.createNestedObject("modifiers");
    modifiersProp["type"] = "string";
    modifiersProp["description"] = "Modifier keys (ctrl, shift, alt, gui)";
    JsonArray keyboardKeyRequired = keyboardKeySchema.createNestedArray("required");
    keyboardKeyRequired.add("key");

    // Keyboard Shortcut Tool
    JsonObject keyboardShortcut = tools.createNestedObject();
    keyboardShortcut["name"] = TOOL_KEYBOARD_SHORTCUT;
    keyboardShortcut["description"] = "Send a keyboard shortcut combination";
    JsonObject keyboardShortcutSchema = keyboardShortcut.createNestedObject("inputSchema");
    keyboardShortcutSchema["type"] = "object";
    JsonObject keyboardShortcutProps = keyboardShortcutSchema.createNestedObject("properties");
    JsonObject shortcutProp = keyboardShortcutProps.createNestedObject("shortcut");
    shortcutProp["type"] = "string";
    shortcutProp["description"] = "Shortcut in ctrl+alt+delete format";
    JsonArray keyboardShortcutRequired = keyboardShortcutSchema.createNestedArray("required");
    keyboardShortcutRequired.add("shortcut");
    
    // Mouse Move Tool
    JsonObject mouseMove = tools.createNestedObject();
    mouseMove["name"] = TOOL_MOUSE_MOVE;
    mouseMove["description"] = "Move the mouse cursor";
    JsonObject mouseMoveSchema = mouseMove.createNestedObject("inputSchema");
    mouseMoveSchema["type"] = "object";
    JsonObject mouseMoveProps = mouseMoveSchema.createNestedObject("properties");
    JsonObject xProp = mouseMoveProps.createNestedObject("x");
    xProp["type"] = "integer";
    xProp["description"] = "X coordinate or relative movement";
    JsonObject yProp = mouseMoveProps.createNestedObject("y");
    yProp["type"] = "integer";
    yProp["description"] = "Y coordinate or relative movement";
    JsonObject relativeProp = mouseMoveProps.createNestedObject("relative");
    relativeProp["type"] = "boolean";
    relativeProp["description"] = "Whether movement is relative (default: true)";
    JsonArray mouseMoveRequired = mouseMoveSchema.createNestedArray("required");
    mouseMoveRequired.add("x");
    mouseMoveRequired.add("y");
    
    // Mouse Click Tool
    JsonObject mouseClick = tools.createNestedObject();
    mouseClick["name"] = TOOL_MOUSE_CLICK;
    mouseClick["description"] = "Click mouse button";
    JsonObject mouseClickSchema = mouseClick.createNestedObject("inputSchema");
    mouseClickSchema["type"] = "object";
    JsonObject mouseClickProps = mouseClickSchema.createNestedObject("properties");
    JsonObject buttonProp = mouseClickProps.createNestedObject("button");
    buttonProp["type"] = "string";
    buttonProp["description"] = "Mouse button (left, right, middle)";
    JsonObject durationProp = mouseClickProps.createNestedObject("duration");
    durationProp["type"] = "integer";
    durationProp["description"] = "Click duration in milliseconds";
    
    // Mouse Scroll Tool
    JsonObject mouseScroll = tools.createNestedObject();
    mouseScroll["name"] = TOOL_MOUSE_SCROLL;
    mouseScroll["description"] = "Scroll the mouse wheel";
    JsonObject mouseScrollSchema = mouseScroll.createNestedObject("inputSchema");
    mouseScrollSchema["type"] = "object";
    JsonObject mouseScrollProps = mouseScrollSchema.createNestedObject("properties");
    JsonObject scrollProp = mouseScrollProps.createNestedObject("scroll");
    scrollProp["type"] = "integer";
    scrollProp["description"] = "Scroll amount (positive = up, negative = down)";
    JsonArray mouseScrollRequired = mouseScrollSchema.createNestedArray("required");
    mouseScrollRequired.add("scroll");
    
    // System Status Tool
    JsonObject systemStatus = tools.createNestedObject();
    systemStatus["name"] = TOOL_SYSTEM_STATUS;
    systemStatus["description"] = "Get system status information";
    JsonObject systemStatusSchema = systemStatus.createNestedObject("inputSchema");
    systemStatusSchema["type"] = "object";
    systemStatusSchema.createNestedObject("properties");
    
    sendMCPResponse(clientId, response);
}

void MCPServer::handleCallTool(uint8_t clientId, const DynamicJsonDocument& request) {
    String toolName = request["params"]["name"];
    JsonVariantConst args = request["params"]["args"];
    int requestId = request["id"] | 0;
    
    if (!hidController) {
        sendMCPError(clientId, requestId, "HID controller not available");
        return;
    }
    
    DynamicJsonDocument result(1024);
    
    if (toolName == TOOL_KEYBOARD_TYPE) {
        result = executeKeyboardType(args);
    } else if (toolName == TOOL_KEYBOARD_KEY) {
        result = executeKeyboardKey(args);
    } else if (toolName == TOOL_KEYBOARD_SHORTCUT) {
        result = executeKeyboardShortcut(args);
    } else if (toolName == TOOL_MOUSE_MOVE) {
        result = executeMouseMove(args);
    } else if (toolName == TOOL_MOUSE_CLICK) {
        result = executeMouseClick(args);
    } else if (toolName == TOOL_MOUSE_SCROLL) {
        result = executeMouseScroll(args);
    } else if (toolName == TOOL_SYSTEM_STATUS) {
        result = executeSystemStatus(args);
    } else {
        sendMCPError(clientId, requestId, "Unknown tool: " + toolName);
        return;
    }
    
    DynamicJsonDocument response(JSON_DOC_SIZE);
    response["jsonrpc"] = "2.0";
    response["id"] = requestId;
    response["result"] = result;
    
    sendMCPResponse(clientId, response);
}

DynamicJsonDocument MCPServer::executeKeyboardType(const JsonVariantConst& args) {
    DynamicJsonDocument result(512);
    String text = args["text"];
    
    bool success = hidController->typeText(text);
    result["success"] = success;
    result["message"] = success ? "Text typed successfully" : "Failed to type text";
    result["typed_text"] = text;
    
    return result;
}

DynamicJsonDocument MCPServer::executeKeyboardKey(const JsonVariantConst& args) {
    DynamicJsonDocument result(512);
    String key = args["key"];
    String modifiers = args["modifiers"] | "";

    bool success = hidController->sendKeyStroke(key, modifiers);
    result["success"] = success;
    result["message"] = success ? "Key pressed successfully" : "Failed to press key";
    result["key"] = key;
    result["modifiers"] = modifiers;

    return result;
}

DynamicJsonDocument MCPServer::executeKeyboardShortcut(const JsonVariantConst& args) {
    DynamicJsonDocument result(512);
    String shortcut = args["shortcut"];

    if (!shortcut.length()) {
        result["success"] = false;
        result["message"] = "Missing shortcut";
        return result;
    }

    String normalizedShortcut = shortcut;
    normalizedShortcut.trim();

    int lastPlus = normalizedShortcut.lastIndexOf('+');
    String key;
    String modifiers;

    if (lastPlus >= 0) {
        key = normalizedShortcut.substring(lastPlus + 1);
        String modifierSection = normalizedShortcut.substring(0, lastPlus);

        // Split modifiers on '+' and rebuild space-delimited string for HID controller
        modifierSection.trim();
        int start = 0;
        while (start >= 0) {
            int nextPlus = modifierSection.indexOf('+', start);
            String token;
            if (nextPlus >= 0) {
                token = modifierSection.substring(start, nextPlus);
                start = nextPlus + 1;
            } else {
                token = modifierSection.substring(start);
                start = -1;
            }
            token.trim();
            token.toLowerCase();
            if (token.length()) {
                if (modifiers.length()) {
                    modifiers += ' ';
                }
                modifiers += token;
            }
        }
    } else {
        key = normalizedShortcut;
    }

    key.trim();

    bool success = hidController->sendKeyStroke(key, modifiers);
    result["success"] = success;
    result["message"] = success ? "Shortcut sent successfully" : "Failed to send shortcut";
    result["shortcut"] = shortcut;
    result["key"] = key;
    result["modifiers"] = modifiers;

    return result;
}

DynamicJsonDocument MCPServer::executeMouseMove(const JsonVariantConst& args) {
    DynamicJsonDocument result(512);
    int16_t x = args["x"];
    int16_t y = args["y"];
    bool relative = args["relative"] | true;
    
    bool success = hidController->moveMouse(x, y, relative);
    result["success"] = success;
    result["message"] = success ? "Mouse moved successfully" : "Failed to move mouse";
    result["x"] = x;
    result["y"] = y;
    result["relative"] = relative;
    
    return result;
}

DynamicJsonDocument MCPServer::executeMouseClick(const JsonVariantConst& args) {
    DynamicJsonDocument result(512);
    String button = args["button"] | "left";
    uint16_t duration = args["duration"] | 50;
    
    uint8_t buttonCode = MOUSE_LEFT;
    if (button == "right") buttonCode = MOUSE_RIGHT;
    else if (button == "middle") buttonCode = MOUSE_MIDDLE;
    
    bool success = hidController->clickMouse(buttonCode, duration);
    result["success"] = success;
    result["message"] = success ? "Mouse clicked successfully" : "Failed to click mouse";
    result["button"] = button;
    result["duration"] = duration;
    
    return result;
}

DynamicJsonDocument MCPServer::executeMouseScroll(const JsonVariantConst& args) {
    DynamicJsonDocument result(512);
    int8_t scroll = args["scroll"];
    
    bool success = hidController->scrollMouse(scroll);
    result["success"] = success;
    result["message"] = success ? "Mouse scrolled successfully" : "Failed to scroll mouse";
    result["scroll"] = scroll;
    
    return result;
}

DynamicJsonDocument MCPServer::executeSystemStatus(const JsonVariantConst& args) {
    DynamicJsonDocument result(1024);
    
    result["success"] = true;
    result["server_name"] = MCP_SERVER_NAME;
    result["server_version"] = MCP_SERVER_VERSION;
    result["protocol_version"] = MCP_PROTOCOL_VERSION;
    result["hid_ready"] = hidController ? hidController->isReady() : false;
    result["wifi_connected"] = WiFi.isConnected();
    result["wifi_ip"] = WiFi.localIP().toString();
    result["wifi_ssid"] = WiFi.SSID();
    result["free_heap"] = ESP.getFreeHeap();
    result["uptime_ms"] = millis();
    
    return result;
}
