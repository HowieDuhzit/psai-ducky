#ifndef MCP_SERVER_H
#define MCP_SERVER_H

#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// Use DynamicJsonDocument instead of JsonDocument
#define JSON_DOC_SIZE 2048
#include "hid_controller.h"

class MCPServer {
private:
    WebSocketsServer* webSocket;
    HIDController* hidController;
    bool isInitialized;
    
    // MCP Protocol handling
    void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
    void handleMCPMessage(uint8_t clientId, const String& message);
    void sendMCPResponse(uint8_t clientId, const DynamicJsonDocument& response);
    void sendMCPError(uint8_t clientId, int requestId, const String& error);
    
    // MCP Protocol methods
    void handleInitialize(uint8_t clientId, const DynamicJsonDocument& request);
    void handleListTools(uint8_t clientId, const DynamicJsonDocument& request);
    void handleCallTool(uint8_t clientId, const DynamicJsonDocument& request);
    
    // Tool implementations
    DynamicJsonDocument executeKeyboardType(const JsonVariantConst& args);
    DynamicJsonDocument executeKeyboardKey(const JsonVariantConst& args);
    DynamicJsonDocument executeKeyboardShortcut(const JsonVariantConst& args);
    DynamicJsonDocument executeMouseMove(const JsonVariantConst& args);
    DynamicJsonDocument executeMouseClick(const JsonVariantConst& args);
    DynamicJsonDocument executeMouseScroll(const JsonVariantConst& args);
    DynamicJsonDocument executeSystemStatus(const JsonVariantConst& args);
    
    // Utility methods
    bool authenticateClient(const String& apiKey);
    String generateToolSchema();
    
public:
    MCPServer(WebSocketsServer* ws);
    ~MCPServer();
    
    bool begin();
    void loop();
    void setHIDController(HIDController* controller);
    
    // Static callback wrapper
    static void webSocketEventWrapper(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
    static MCPServer* instance;
};

#endif // MCP_SERVER_H