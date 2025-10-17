# psAI-Ducky (ESP32-S2 HID MCP Bridge)

Remote control any PC via WiFi → ESP32 → USB HID.  
MCP-compatible bridge for ESP32-based keyboard and mouse control.

## Architecture

```
Your Computer ──WiFi──→ ESP32 ──USB──→ Target PC
     ↑                    ↑              ↑
 Claude Desktop      MCP Server    Keyboard/Mouse
```

## Quick Start

### 1) Firmware build and flash (ESP32-S3)
- Copy credentials (optional) and build:
  ```bash
  cp config/wifi_credentials.h.example config/wifi_credentials.h   # then edit values
  platformio run -e esp32-s3-devkitc-1 -t upload
  ```
- If no credentials are provided, the device starts an AP: SSID `ESP32-MCP-Server`, password `esp32mcp123`.

### 2) Run the MCP bridge
```
node index.js
```
- Configure env if needed:
  - `ESP32_HOST` (default: 192.168.4.1 in AP mode)
  - `ESP32_PORT` (default: 8080)

### 3) Claude Desktop
Add to `~/.config/claude-desktop/config.json`:
```json
{
  "mcpServers": {
    "esp32-hid-controller": {
      "command": "node",
      "args": ["/home/howie/GitHub/ESPCP/index.js"],
      "env": { "ESP32_HOST": "192.168.4.1", "ESP32_PORT": "8080" }
    }
  }
}
```

## Tools
- keyboard_type: Type text
- keyboard_key: Press a key (with optional modifiers)
- keyboard_shortcut: Send a shortcut (e.g., ctrl+alt+delete)
- mouse_move: Move cursor
- mouse_click: Click button
- mouse_scroll: Scroll wheel
- system_status: ESP32 status

## Testing
- Node smoke test:
  ```bash
  npm test
  ```
- Manual init:
  ```bash
  echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{"tools":true}}}' | node index.js
  ```

## Project layout
```
ESPCP/
├── include/                # Firmware headers
├── src/                    # Firmware sources
├── platformio.ini          # PlatformIO config
├── index.js                # Node MCP bridge
├── package.json
└── config/wifi_credentials.h.example
```

## Troubleshooting
- If the bridge shows no tools, confirm the ESP32 WebSocket at `ws://<ip>:8080` is reachable.
- If HID doesn’t work, ensure the board is ESP32-S3 and is connected by USB to the target PC (not just powered).

## Branding and Credits
psAI-Ducky — a Psyduck x AI x USB Rubber Ducky mashup.

Created by Howie Duhzit
- Web: https://HowieDuhzit.Best
- X: @HowieDuhzit
- Discord: https://discord.gg/ZFp9wegP
- Email: Contact@HowieDuhzit.Best
- Warp referral: https://app.warp.dev/referral/3E9X3D

Buy me a coffee:
<script type="text/javascript" src="https://cdnjs.buymeacoffee.com/1.0.0/button.prod.min.js" data-name="bmc-button" data-slug="howieduhzit" data-color="#FFDD00" data-emoji="🥤"  data-font="Poppins" data-text="Buy me an energy drink" data-outline-color="#000000" data-font-color="#000000" data-coffee-color="#ffffff" ></script>
