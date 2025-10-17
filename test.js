#!/usr/bin/env node

class MockWebSocket {
    constructor() {
        this.handlers = {};
    }

    on(event, handler) {
        this.handlers[event] = handler;
    }

    off(event) {
        delete this.handlers[event];
    }

    send() {}

    close() {}
}

global.__ESP32_MCP_MOCKS__ = { WebSocket: MockWebSocket };

const ESP32MCPServer = require('./index');

(async () => {
    const server = new ESP32MCPServer();

    server.initialized = true;
    server.esp32Ws = new MockWebSocket();
    server.sendToESP32 = async () => ({
        jsonrpc: '2.0',
        id: 1,
        result: {
            tools: [
                { name: 'keyboard_type' },
                { name: 'keyboard_key' },
                { name: 'keyboard_shortcut' }
            ]
        }
    });

    const tools = await server.getTools();

    if (!Array.isArray(tools)) {
        console.error('Expected tools to be an array');
        process.exit(1);
    }

    if (tools.length !== 3) {
        console.error(`Expected 3 tools, received ${tools.length}`);
        process.exit(1);
    }

    if (!tools.find((tool) => tool.name === 'keyboard_shortcut')) {
        console.error('Expected keyboard_shortcut tool to be present');
        process.exit(1);
    }

    console.log('✅ MCP tool reporting logic passed smoke test');
})();
