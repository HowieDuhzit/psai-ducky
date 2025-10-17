const WebSocket = require('ws');

const ws = new WebSocket('ws://192.168.8.186:8080');

ws.on('open', () => {
  console.error('✅ Connected to ESP32');
  
  const initMsg = {
    jsonrpc: '2.0',
    id: 1,
    method: 'initialize',
    params: {
      protocolVersion: '2024-11-05',
      capabilities: { tools: true },
      clientInfo: { name: 'test', version: '1.0.0' }
    }
  };
  
  ws.send(JSON.stringify(initMsg));
});

ws.on('message', (data) => {
  const msg = JSON.parse(data.toString());
  console.log('Response:', JSON.stringify(msg, null, 2));
  
  if (msg.id === 1 && msg.result) {
    console.error('✅ Initialized, testing keyboard...');
    ws.send(JSON.stringify({
      jsonrpc: '2.0',
      id: 2,
      method: 'tools/call',
      params: {
        name: 'keyboard_type',
        args: { text: 'ESP32 TEST ' }
      }
    }));
  } else if (msg.id === 2) {
    console.error('✅ Test complete');
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
}, 10000);
