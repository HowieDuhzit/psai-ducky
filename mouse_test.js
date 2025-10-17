const WebSocket = require('ws');
const ws = new WebSocket('ws://192.168.8.186:8080');

ws.on('open', () => {
  ws.send(JSON.stringify({ jsonrpc: '2.0', id: 1, method: 'initialize', params: { protocolVersion: '2024-11-05', capabilities: { tools: true } } }));
});

ws.on('message', (d) => {
  const m = JSON.parse(d.toString());
  if (m.id === 1) {
    // Move cursor diagonally
    ws.send(JSON.stringify({ jsonrpc: '2.0', id: 2, method: 'tools/call', params: { name: 'mouse_move', args: { x: 100, y: 100, relative: true } } }));
  } else if (m.id === 2) {
    // Left click
    ws.send(JSON.stringify({ jsonrpc: '2.0', id: 3, method: 'tools/call', params: { name: 'mouse_click', args: { button: 'left', duration: 60 } } }));
  } else if (m.id === 3) {
    // Scroll down
    ws.send(JSON.stringify({ jsonrpc: '2.0', id: 4, method: 'tools/call', params: { name: 'mouse_scroll', args: { scroll: -40 } } }));
  } else if (m.id === 4) {
    // Move back
    ws.send(JSON.stringify({ jsonrpc: '2.0', id: 5, method: 'tools/call', params: { name: 'mouse_move', args: { x: -100, y: -100, relative: true } } }));
  } else if (m.id === 5) {
    console.log('Mouse test complete:', JSON.stringify(m.result));
    ws.close();
  }
});

ws.on('error', e => { console.error('WS error:', e.message); process.exit(1); });
setTimeout(() => process.exit(1), 15000);
