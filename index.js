#!/usr/bin/env node

/*
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
██████████████████████████████████████████▂▂▂▂▂▂▂▂▂▂▂███████████████████████████████████████████████
████████████████████████████████████▇▃▉▎▏▏           ▏▏▏▊▄▇█████████████████████████████████████████
██████████████████████████████████▇▁▎                     ▏▋▆███████████████████████████████████████
█████████████████████████████████▃▎                         ▏▉██████████████████████████████████████
████████████████████████████████▂                            ▏▁▇████████████████████████████████████
███████████████████████████████▃▏                              ▉████████████████████████████████████
██████████████████████████████▃▎  ▎▊▂▄▁▎               ▏▌▁▃▄▋  ▏▂███████████████████████████████████
██████████████████████████████▊  ▌▅▍▍██▆▍              ▄▋ ▄██▂  ▋███████████████████████████████████
██████████████████████████████▊  ▋██████▍              ▇█████▅  ▋███████████████████████████████████
██████████████████████████████▊  ▏▉▇██▇▉▏ ▏▏▋▊▊▊▊▊▊▍▏  ▌▇███▆▍  ▌███████████████████████████████████
█████████████████████████████▃▍    ▍▋▋▍ ▏▋▉▊▍   ▏▌▍▅▄▂▌ ▏▋▋▋▏   ▎▃██████████████████████████████████
█████████████████████████████▊     ▎▍▍▊▉▊▌▏▂▁     ▊▄▌▁▅▄▉▍▍▍     ▌▇█████████████████████████████████
█████████████████████████████▊    ▉▋▍▍▎    ▏▏     ▏▏▏ ▎▊▃▆▇█▄▎   ▎▆█████████████████████████████████
█████████████████████████████▃▎   ▆▋▌▉▍▏ ▏    ▏ ▏▏▏▏▏   ▊▁▆█▇▍  ▏▂██████████████████████████████████
██████████████████████████████▅▊▏ ▏▊▅▅▇▃▃▆▆▃▂▂▃▅▅▅████▃▅█▅▅▁▎  ▍▄███████████████████████████████████
████████████████████████████████▅▊▍   ▉███▆▅▅▅▅▅▅▅▅▅▇███▄   ▏▋▃█████████████████████████████████████
██████████████████████████████████▇▅▉▏▏▎▂█▆▄▄▅▅▅▅▅▅▅██▄▍▎ ▋▆▆███████████████████████████████████████
██████████████████████████████████████   ▏▎▍▌▁▉▉▄▅▂▊▉▍▏  ▃██████████████████████████████████████████
██████████████████████████████████████                 ▏▏▃██████████████████████████████████████████
████████████████████████████████████▅▌                 ▏▏▊▇█████████████████████████████████████████
██████████████████████████████████▇▊▎                    ▏▎▁▇██████████████▆▋▁██████████████████████
███████████████████████████████▃▁▏▏                         ▏▏▌▂▂▄▇██████▆▂▍ ▏▊▇████████████████████
███████████████████████████▄▉▁▉▎                                               ▄████████████████████
█████████████████████████▆▌▏▊▊                          ▌▊▊▊▊▊▊▊▊▊▊▌▏         ▏▃████████████████████
█████████████████████████▎▎▁▊                         ▏▁▌▏        ▎▍▉▉▉▉▉▉▉▍   ▏▇███████████████████
████████████████████████▌▏▂▋                         ▏▄▏             ▏▏   ▏▆▊   ▇███████████████████
████████████████████████▍▄▋                          ▎▄                    ▅▊  ▋▇███████████████████
████████████████████████▍▆                           ▎▅                   ▍▅▍  ▆████████████████████
████████████████████████▍▋                           ▏▋▂                ▏▋█▋  ▎▆████████████████████
████████████████████████▃▁                            ▏▍▃▋▏▏           ▎▂▂▍  ▏▄█████████████████████
█████████████████████████▅▉                             ▏▌▉▁▉▉▁▁▁▁▁▂▂▁▉▊▎   ▍▇██████████████████████
█████████████████████████▆▅                                   ▏▎▎▏▏▎▌    ▏▎▁▅███████████████████████
██████████████████████████▆▋▍                                          ▏▏▎▃▇████████████████████████
████████████████████████████▇▉▎                                     ▏  ▋▃█▇█████████████████████████
█████████████████████████████▇██▃▂▃▁▊▎  ▏▏  ▏         ▏▏▏▏▏▏▉▉▍▍▉▉▎▎▋▁▇█▇███████████████████████████
█████████████████████████████████████▆▂▃▃▁▉▋▋▋▋▋▋▋▋▋▋▋▊▂▆▇▄▂▇█▇▅▂▂▆▅▅▇██████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████████████████████

psAI-Ducky — ESP32-S2 HID MCP Bridge (Node client)
(c) 2025 Howie Duhzit — HowieDuhzit.Best — @HowieDuhzit — Contact@HowieDuhzit.Best
*/

/**
 * ESP32 HID MCP Server
 * NPX-compatible Node.js wrapper for ESP32 HID remote control
 * Bridges MCP protocol over stdio to ESP32 WebSocket server
 */

const MockedWebSocket = globalThis.__ESP32_MCP_MOCKS__?.WebSocket;
const WebSocket = MockedWebSocket || require('ws');
const readline = require('readline');

class ESP32MCPServer {
    constructor() {
this.esp32Host = process.env.ESP32_HOST || '192.168.4.1';
        this.esp32Port = process.env.ESP32_PORT || '8080';
        this.esp32Ws = null;
        this.initialized = false;
        this.requestId = 1;
        this.connectPromise = null;
        
        // Setup stdio interface
        this.rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout,
            terminal: false
        });
        
        this.setupSignalHandlers();
    }
    
    setupSignalHandlers() {
        process.on('SIGINT', () => this.cleanup());
        process.on('SIGTERM', () => this.cleanup());
        process.on('exit', () => this.cleanup());
    }
    
    cleanup() {
        if (this.esp32Ws) {
            try {
                this.esp32Ws.close();
            } catch (error) {
                console.error(`⚠️  Error closing ESP32 socket: ${error.message}`);
            }
        }
        if (this.rl) {
            this.rl.close();
        }
        this.esp32Ws = null;
        this.initialized = false;
    }
    
    async connectToESP32() {
        if (this.initialized && this.esp32Ws) {
            return true;
        }

        if (this.connectPromise) {
            return this.connectPromise;
        }

        const wsUrl = `ws://${this.esp32Host}:${this.esp32Port}`;
        
        this.connectPromise = new Promise((resolve, reject) => {
            const socket = new WebSocket(wsUrl);
            this.esp32Ws = socket;

            const handleRuntimeError = (error) => {
                console.error(`⚠️  ESP32 socket error: ${error.message}`);
            };

            const handleClose = () => {
                socket.off('error', handleRuntimeError);
                if (this.esp32Ws === socket) {
                    this.initialized = false;
                    this.esp32Ws = null;
                }
            };

            const handleError = (error) => {
                clearTimeout(timeout);
                socket.off('open', handleOpen);
                handleClose();
                try {
                    socket.close();
                } catch (closeError) {
                    console.error(`⚠️  Error closing ESP32 socket after failure: ${closeError.message}`);
                }
                reject(error instanceof Error ? error : new Error(String(error)));
            };

            const timeout = setTimeout(() => {
                socket.off('open', handleOpen);
                socket.off('error', handleError);
                socket.close();
                handleClose();
                reject(new Error('Connection timeout'));
            }, 5000);

            const handleOpen = async () => {
                socket.off('open', handleOpen);
                clearTimeout(timeout);
                try {
                    socket.off('error', handleError);
                    socket.on('error', handleRuntimeError);
                    const response = await this.sendToESP32({
                        method: 'initialize',
                        params: {
                            protocolVersion: '2024-11-05',
                            capabilities: { tools: true },
                            clientInfo: { name: 'esp32-hid-mcp-bridge', version: '1.0.0' }
                        }
                    }, { skipEnsure: true });

                    if (response.result) {
                        this.initialized = true;
                        resolve(true);
                    } else {
                        handleClose();
                        try {
                            socket.close();
                        } catch (closeError) {
                            console.error(`⚠️  Error closing ESP32 socket after init failure: ${closeError.message}`);
                        }
                        reject(new Error('ESP32 initialization failed'));
                    }
                } catch (error) {
                    socket.off('error', handleRuntimeError);
                    handleClose();
                    try {
                        socket.close();
                    } catch (closeError) {
                        console.error(`⚠️  Error closing ESP32 socket during init catch: ${closeError.message}`);
                    }
                    reject(error);
                }
            };

            socket.once('open', handleOpen);
            socket.once('error', handleError);
            socket.on('close', handleClose);
        });

        try {
            return await this.connectPromise;
        } finally {
            this.connectPromise = null;
        }
    }

    async ensureConnected() {
        if (this.initialized && this.esp32Ws) {
            return true;
        }
        try {
            await this.connectToESP32();
        } catch (error) {
            console.error(`⚠️  ESP32 connection failed: ${error.message}`);
        }
        return this.initialized;
    }

    async sendToESP32(message, options = {}) {
        const { skipEnsure = false } = options;

        if (!skipEnsure) {
            const connected = await this.ensureConnected();
            if (!connected) {
                throw new Error('Not connected to ESP32');
            }
        } else if (!this.esp32Ws) {
            throw new Error('Not connected to ESP32');
        }

        const request = {
            jsonrpc: '2.0',
            id: message.id || this.requestId++,
            method: message.method,
            params: message.params || {}
        };

        return new Promise((resolve, reject) => {
            const timeout = setTimeout(() => {
                this.esp32Ws.off('message', messageHandler);
                reject(new Error('ESP32 request timeout'));
            }, 10000);

            const messageHandler = (data) => {
                clearTimeout(timeout);
                this.esp32Ws.off('message', messageHandler);

                try {
                    const response = JSON.parse(data.toString());
                    resolve(response);
                } catch (error) {
                    reject(error);
                }
            };

            try {
                this.esp32Ws.on('message', messageHandler);
                this.esp32Ws.send(JSON.stringify(request));
            } catch (error) {
                clearTimeout(timeout);
                this.esp32Ws.off('message', messageHandler);
                reject(error);
            }
        });
    }

    async getTools() {
        const response = await this.sendToESP32({
            method: 'tools/list',
            params: {}
        });
        return this.extractTools(response);
    }

    async callTool(toolName, args) {
        const response = await this.sendToESP32({
            method: 'tools/call',
            params: {
                name: toolName,
                args: args
            }
        });
        return response;
    }

    extractTools(response) {
        if (!response) {
            return [];
        }

        if (Array.isArray(response.tools)) {
            return response.tools;
        }

        const nestedTools = response?.result?.tools;
        return Array.isArray(nestedTools) ? nestedTools : [];
    }

    getFallbackTools() {
        return [
            {
                name: "keyboard_type",
                description: "Type text using the keyboard",
                inputSchema: {
                    type: "object",
                    properties: {
                        text: {
                            type: "string",
                            description: "Text to type"
                        }
                    },
                    required: ["text"]
                }
            },
            {
                name: "keyboard_key",
                description: "Press a specific key or key combination",
                inputSchema: {
                    type: "object",
                    properties: {
                        key: {
                            type: "string",
                            description: "Key to press (e.g., 'a', 'Enter', 'F1')"
                        },
                        modifiers: {
                            type: "string",
                            description: "Modifier keys (ctrl, shift, alt, gui)"
                        }
                    },
                    required: ["key"]
                }
            },
            {
                name: "keyboard_shortcut",
                description: "Send a keyboard shortcut combination",
                inputSchema: {
                    type: "object",
                    properties: {
                        shortcut: {
                            type: "string",
                            description: "Shortcut in ctrl+alt+delete format"
                        }
                    },
                    required: ["shortcut"]
                }
            },
            {
                name: "mouse_move",
                description: "Move the mouse cursor",
                inputSchema: {
                    type: "object",
                    properties: {
                        x: {
                            type: "integer",
                            description: "X coordinate or relative movement"
                        },
                        y: {
                            type: "integer",
                            description: "Y coordinate or relative movement"
                        },
                        relative: {
                            type: "boolean",
                            description: "Whether movement is relative (default: true)"
                        }
                    },
                    required: ["x", "y"]
                }
            },
            {
                name: "mouse_click",
                description: "Click mouse button",
                inputSchema: {
                    type: "object",
                    properties: {
                        button: {
                            type: "string",
                            description: "Mouse button (left, right, middle)"
                        },
                        duration: {
                            type: "integer",
                            description: "Click duration in milliseconds"
                        }
                    }
                }
            },
            {
                name: "mouse_scroll",
                description: "Scroll the mouse wheel",
                inputSchema: {
                    type: "object",
                    properties: {
                        scroll: {
                            type: "integer",
                            description: "Scroll amount (positive = up, negative = down)"
                        }
                    },
                    required: ["scroll"]
                }
            },
            {
                name: "system_status",
                description: "Get system status information",
                inputSchema: {
                    type: "object",
                    properties: {}
                }
            }
        ];
    }
    
    async handleMCPRequest(request) {
        const { method, id = 0, params = {} } = request;
        
        try {
            switch (method) {
                case 'initialize':
                    // Use the same protocol version as the client
                    const clientProtocolVersion = params.protocolVersion || '2024-11-05';
                    
                    // Try to connect to ESP32 if not already connected
                    const wasInitialized = this.initialized;
                    const connected = await this.ensureConnected();
                    if (connected && !wasInitialized) {
                        console.error('✅ Connected to ESP32');
                    } else if (!connected) {
                        console.error('⚠️  ESP32 not reachable, continuing in fallback mode');
                    }
                    
                    return {
                        jsonrpc: '2.0',
                        id: id,
                        result: {
                            protocolVersion: clientProtocolVersion,
                            capabilities: { tools: true },
                            serverInfo: {
                                name: 'esp32-hid-mcp-bridge',
                                version: '1.0.0'
                            }
                        }
                    };
                
                case 'tools/list':
                    let tools = [];
                    try {
                        if (await this.ensureConnected()) {
                            tools = await this.getTools();
                        } else {
                            tools = this.getFallbackTools();
                        }
                    } catch (error) {
                        console.error(`⚠️  Failed to get tools from ESP32: ${error.message}`);
                        tools = this.getFallbackTools();
                    }
                    
                    return {
                        jsonrpc: '2.0',
                        id: id,
                        result: { tools: tools }
                    };
                
                case 'tools/call':
                    const { name: toolName, args: toolArgs } = params;
                    if (!toolName) {
                        return {
                            jsonrpc: '2.0',
                            id: id,
                            error: { code: -32602, message: 'Missing tool name' }
                        };
                    }
                    
                    if (!(await this.ensureConnected())) {
                        return {
                            jsonrpc: '2.0',
                            id: id,
                            error: { code: -32001, message: 'ESP32 not connected' }
                        };
                    }

                    try {
                        const toolResult = await this.callTool(toolName, toolArgs || {});
                        return {
                            jsonrpc: '2.0',
                            id: id,
                            result: toolResult.result || toolResult
                        };
                    } catch (error) {
                        return {
                            jsonrpc: '2.0',
                            id: id,
                            error: { code: -32002, message: error.message }
                        };
                    }
                
                default:
                    return {
                        jsonrpc: '2.0',
                        id: id,
                        error: { code: -32601, message: `Unknown method: ${method}` }
                    };
            }
        } catch (error) {
            return {
                jsonrpc: '2.0',
                id: id,
                error: { code: -32603, message: error.message }
            };
        }
    }
    
    async start() {
        console.error('🚀 ESP32 HID MCP Server starting...');
        console.error(`📡 ESP32 target: ws://${this.esp32Host}:${this.esp32Port}`);
        
        // Handle MCP requests from stdin
        this.rl.on('line', async (line) => {
            if (!line.trim()) return;
            
            try {
                const request = JSON.parse(line);
                const response = await this.handleMCPRequest(request);
                console.log(JSON.stringify(response));
            } catch (error) {
                console.error(`❌ Error processing request: ${error.message}`);
                const errorResponse = {
                    jsonrpc: '2.0',
                    id: 0,
                    error: { code: -32700, message: 'Parse error' }
                };
                console.log(JSON.stringify(errorResponse));
            }
        });
        
        this.rl.on('close', () => {
            this.cleanup();
            process.exit(0);
        });
    }
}

// Start the server if run directly
if (require.main === module) {
    const server = new ESP32MCPServer();
    server.start().catch((error) => {
        console.error(`❌ Server startup failed: ${error.message}`);
        process.exit(1);
    });
}

module.exports = ESP32MCPServer;
