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

## Examples
- Full chat typing and send:
  ```bash
  node examples/full_chat_test.js
  ```
- Mouse exercise:
  ```bash
  node examples/mouse_test.js
  ```
- Direct HID typing:
  ```bash
  node examples/test_esp32_hid.js
  ```

## Board support matrix

### ESP32-S2 Boards (Native USB HID Support) ✅

| Board                         | PlatformIO Env         | USB HID | Build Status | Tested | Notes |
|-------------------------------|------------------------|---------|--------------|--------|-------|
| **Flipper Zero WiFi Dev**     | `esp32-s2-flipper`     | ✅ Yes   | ![Build][flipper] | ✅ Yes  | **Primary test board** |
| ESP32-S2 Saola-1              | `esp32-s2-saola-1`     | ✅ Yes   | ![Build][s2-saola] | ✅ Yes  | Reference design |
| ESP32-S2 DevKitM-1            | `esp32-s2-devkitm-1`   | ✅ Yes   | ![Build][s2-devkitm] | ⏳ Pending | Standard dev board |
| Adafruit Feather ESP32-S2     | `adafruit_feather_esp32s2` | ✅ Yes | ![Build][ada-s2] | ⏳ Pending | Compact feather form |
| Unexpected Maker FeatherS2    | `um_feathers2`         | ✅ Yes   | ![Build][um-s2] | ⏳ Pending | Premium S2 board |

### ESP32-S3 Boards (Native USB HID Support) ✅

| Board                         | PlatformIO Env         | USB HID | Build Status | Tested | Notes |
|-------------------------------|------------------------|---------|--------------|--------|-------|
| ESP32-S3 DevKitC-1            | `esp32-s3-devkitc-1`   | ✅ Yes   | ![Build][s3-devkitc] | ⏳ Pending | Standard S3 dev board |
| ESP32-S3 DevKitC-1 (8MB+8MB)  | `esp32-s3-devkitc-1-n8r8` | ✅ Yes | ![Build][s3-n8r8] | ⏳ Pending | With PSRAM |
| Adafruit Feather ESP32-S3     | `adafruit_feather_esp32s3` | ✅ Yes | ![Build][ada-s3] | ⏳ Pending | Compact feather form |
| Unexpected Maker TinyS3       | `um_tinys3`            | ✅ Yes   | ![Build][um-tinys3] | ⏳ Pending | Tiny S3 board |
| LilyGo T-Display-S3           | `lilygo-t-display-s3`  | ✅ Yes   | ![Build][lilygo-s3] | ⏳ Pending | With built-in display |

### ESP32 Classic & C3/C6 Boards (No Native USB HID) ⚠️

| Board                         | PlatformIO Env         | USB HID | Build Status | Tested | Notes |
|-------------------------------|------------------------|---------|--------------|--------|-------|
| ESP32 DevKit (WROOM-32)       | `esp32dev`             | ❌ No    | ![Build][esp32] | N/A | BLE HID planned |
| ESP32-C3 DevKitM-1            | `esp32-c3-devkitm-1`   | ❌ No    | ![Build][c3-m1] | N/A | USB-Serial/JTAG only |
| ESP32-C3 DevKitC-02           | `esp32-c3-devkitc-02`  | ❌ No    | ![Build][c3-c02] | N/A | USB-Serial/JTAG only |
| Seeed XIAO ESP32-C3           | `seeed_xiao_esp32c3`   | ❌ No    | ![Build][xiao-c3] | N/A | Tiny form factor |
| ESP32-C6 DevKitC-1            | `esp32-c6-devkitc-1`   | ❌ No    | ![Build][c6] | N/A | Newest chip, BLE planned |

**Legend:**
- ✅ = Fully supported
- ⏳ = Not yet tested on hardware (builds successfully)
- ❌ = Not supported (use BLE HID when implemented)
- ![Build][badge] = Build status from GitHub Actions

**Note:** For non-native USB targets, HID functionality is stubbed. MCP server still works for status monitoring. BLE HID support is planned for future releases.

**Want to contribute?** PRs welcome for:
- Testing builds on untested boards
- Adding new board support
- Implementing BLE HID for non-USB boards

[flipper]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[s2-saola]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[s2-devkitm]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[ada-s2]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[um-s2]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[s3-devkitc]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[s3-n8r8]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[ada-s3]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[um-tinys3]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[lilygo-s3]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[esp32]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[c3-m1]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[c3-c02]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[xiao-c3]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev
[c6]: https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev

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
