const WebSocket = require('ws');

const ws = new WebSocket('ws://192.168.8.186:8080');
let step = 0;

ws.on('open', () => {
  console.error('🚀 Connected to ESP32 HID Controller');
  
  // Initialize
  ws.send(JSON.stringify({
    jsonrpc: '2.0',
    id: 1,
    method: 'initialize',
    params: {
      protocolVersion: '2024-11-05',
      capabilities: { tools: true },
      clientInfo: { name: 'full-test', version: '1.0.0' }
    }
  }));
});

ws.on('message', (data) => {
  const msg = JSON.parse(data.toString());
  
  if (msg.id === 1 && msg.result) {
    console.error('✅ Initialized');
    console.error('⌨️  Typing test message in chat...');
    
    // Type the test message
    ws.send(JSON.stringify({
      jsonrpc: '2.0',
      id: 2,
      method: 'tools/call',
      params: {
        name: 'keyboard_type',
        args: { text: 'Hello! This message was typed by the ESP32-S2 HID device via MCP. The system is working perfectly! 🎉' }
      }
    }));
  } else if (msg.id === 2) {
    console.error('✅ Message typed');
    console.log('Keyboard response:', JSON.stringify(msg.result, null, 2));
    console.error('📤 Pressing Enter to send...');
    
    // Press Enter to send
    ws.send(JSON.stringify({
      jsonrpc: '2.0',
      id: 3,
      method: 'tools/call',
      params: {
        name: 'keyboard_key',
        args: { key: 'Enter' }
      }
    }));
  } else if (msg.id === 3) {
    console.error('✅ Enter pressed');
    console.log('Enter response:', JSON.stringify(msg.result, null, 2));
    console.error('🎉 Full test complete!');
    ws.close();
    process.exit(0);
  }
});

ws.on('error', (err) => {
  console.error('❌ Error:', err.message);
  process.exit(1);
});

setTimeout(() => {
  console.error('⏱️  Timeout');
  process.exit(1);
}, 15000);
