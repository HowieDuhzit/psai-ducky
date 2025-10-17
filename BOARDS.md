# ESP32 Board Support Guide

This document provides detailed information about all supported ESP32 boards in the psAI-Ducky project.

## Quick Reference

### Build Command Format
```bash
platformio run -e <environment-name>
platformio run -e <environment-name> -t upload
```

### Native USB HID Boards (Recommended)

These boards support native USB HID and can act as keyboard/mouse without additional hardware:

#### ESP32-S2 Family
- **ESP32-S2 Saola-1**: Reference dev board from Espressif
- **ESP32-S2 DevKitM-1**: Smaller form factor dev board
- **ESP32-S2 Flipper Zero WiFi Dev**: Compact board designed for Flipper Zero (primary test board)
- **Adafruit Feather ESP32-S2**: Battery-friendly feather form factor
- **Unexpected Maker FeatherS2**: Premium S2 board with enhanced features

#### ESP32-S3 Family
- **ESP32-S3 DevKitC-1**: Standard S3 development board
- **ESP32-S3 DevKitC-1 (N8R8)**: DevKitC-1 with 8MB Flash + 8MB PSRAM
- **Adafruit Feather ESP32-S3**: S3 version of feather board
- **Unexpected Maker TinyS3**: Ultra-compact S3 board
- **LilyGo T-Display-S3**: S3 with built-in TFT display

### Non-USB Boards (MCP Only, BLE HID Planned)

These boards cannot act as native USB HID devices but can still run the MCP server:

#### ESP32 Classic
- **ESP32 DevKit (WROOM-32)**: Original ESP32, most common board

#### ESP32-C3 Family  
- **ESP32-C3 DevKitM-1**: Compact RISC-V based board
- **ESP32-C3 DevKitC-02**: Standard C3 dev board
- **Seeed XIAO ESP32-C3**: Tiny breadboard-friendly board

#### ESP32-C6 Family
- **ESP32-C6 DevKitC-1**: Newest chip with WiFi 6 and Zigbee

## Board-Specific Notes

### Flipper Zero WiFi Dev Board (ESP32-S2)
**Status:** Primary test board ✅

**PlatformIO Env:** `esp32-s2-flipper`

**Features:**
- Native USB HID support
- Compact size designed for Flipper Zero
- Compatible with Flipper's expansion port

**Build Command:**
```bash
platformio run -e esp32-s2-flipper -t upload
```

**Notes:**
- Uses Saola-1 board profile as base
- Tested and verified working with psAI-Ducky

---

### ESP32-S2 Saola-1
**Status:** Reference design ✅

**PlatformIO Env:** `esp32-s2-saola-1`

**Features:**
- Official Espressif reference design
- Full GPIO breakout
- Native USB HID support

**Build Command:**
```bash
platformio run -e esp32-s2-saola-1 -t upload
```

---

### ESP32-S3 DevKitC-1
**Status:** Builds successfully, pending hardware test ⏳

**PlatformIO Env:** `esp32-s3-devkitc-1`

**Features:**
- Dual-core Xtensa processor
- Native USB HID support
- More RAM and processing power than S2

**Build Command:**
```bash
platformio run -e esp32-s3-devkitc-1 -t upload
```

**Notes:**
- Should work identically to S2 boards
- More powerful than S2 but also more expensive

---

### ESP32-S3 DevKitC-1 (N8R8)
**Status:** Builds successfully, pending hardware test ⏳

**PlatformIO Env:** `esp32-s3-devkitc-1-n8r8`

**Features:**
- 8MB Flash + 8MB PSRAM
- Enhanced for memory-intensive applications
- Native USB HID support

**Build Command:**
```bash
platformio run -e esp32-s3-devkitc-1-n8r8 -t upload
```

**Notes:**
- PSRAM enabled via `-DBOARD_HAS_PSRAM` flag
- Recommended for future expansion features

---

### Adafruit Feather Boards
**Status:** Builds successfully, pending hardware test ⏳

**PlatformIO Envs:** 
- `adafruit_feather_esp32s2`
- `adafruit_feather_esp32s3`

**Features:**
- Battery charging circuitry
- Compact feather form factor
- Native USB HID support (S2/S3)
- Easy to integrate into portable projects

**Build Commands:**
```bash
platformio run -e adafruit_feather_esp32s2 -t upload
platformio run -e adafruit_feather_esp32s3 -t upload
```

---

### Unexpected Maker Boards
**Status:** Builds successfully, pending hardware test ⏳

**PlatformIO Envs:**
- `um_feathers2` (S2)
- `um_tinys3` (S3)

**Features:**
- Premium build quality
- Enhanced power management
- Native USB HID support
- Multiple color options

**Build Commands:**
```bash
platformio run -e um_feathers2 -t upload
platformio run -e um_tinys3 -t upload
```

---

### LilyGo T-Display-S3
**Status:** Builds successfully, pending hardware test ⏳

**PlatformIO Env:** `lilygo-t-display-s3`

**Features:**
- Built-in 1.9" TFT LCD display
- Native USB HID support
- Battery connector
- Perfect for visual feedback projects

**Build Command:**
```bash
platformio run -e lilygo-t-display-s3 -t upload
```

**Future Plans:**
- Display integration for status/debugging
- Visual confirmation of HID commands

---

### ESP32 Classic (WROOM-32)
**Status:** Builds successfully, HID not supported ⚠️

**PlatformIO Env:** `esp32dev`

**Features:**
- Most common/affordable ESP32 board
- No native USB HID support
- Can run MCP server functionality

**Build Command:**
```bash
platformio run -e esp32dev -t upload
```

**Limitations:**
- Cannot act as USB keyboard/mouse
- BLE HID support planned for future

---

### ESP32-C3 Family
**Status:** Builds successfully, HID not supported ⚠️

**PlatformIO Envs:**
- `esp32-c3-devkitm-1`
- `esp32-c3-devkitc-02`
- `seeed_xiao_esp32c3`

**Features:**
- RISC-V based processor
- Lower cost than S2/S3
- USB-Serial/JTAG built-in
- WiFi & BLE

**Build Commands:**
```bash
platformio run -e esp32-c3-devkitm-1 -t upload
platformio run -e esp32-c3-devkitc-02 -t upload
platformio run -e seeed_xiao_esp32c3 -t upload
```

**Limitations:**
- No native USB HID support
- USB port is for Serial/JTAG only
- BLE HID support planned

---

### ESP32-C6
**Status:** Builds successfully, HID not supported ⚠️

**PlatformIO Env:** `esp32-c6-devkitc-1`

**Features:**
- Newest Espressif chip
- WiFi 6 support
- Zigbee/Thread support
- Enhanced security features

**Build Command:**
```bash
platformio run -e esp32-c6-devkitc-1 -t upload
```

**Limitations:**
- No native USB HID support
- BLE HID support planned

---

## Testing Your Board

### 1. Build Test
```bash
platformio run -e <your-board-env>
```

### 2. Flash & Monitor
```bash
platformio run -e <your-board-env> -t upload -t monitor
```

### 3. Check Status
Navigate to `http://<board-ip>:8080/` in your browser to see the debug page.

### 4. Test MCP Connection
```bash
npm test
```

### 5. Test HID (Native USB boards only)
```bash
node examples/test_esp32_hid.js
```

---

## Contributing Board Support

Want to add support for a new board? Here's how:

### 1. Add PlatformIO Environment

Edit `platformio.ini`:

```ini
[env:my-new-board]
platform = espressif32
board = my-board-id
framework = arduino
monitor_speed = 115200
build_flags = 
    -DESP32_MCP_SERVER
    -DHAVE_NATIVE_USB=1  ; or 0 for non-USB boards
lib_deps = 
    bblanchon/ArduinoJson@^6.21.3
    links2004/WebSockets@^2.4.0
```

### 2. Update GitHub Actions

Edit `.github/workflows/build.yml` and add your environment to the matrix:

```yaml
matrix:
  environment:
    - my-new-board
```

### 3. Update Documentation

Add your board to:
- `README.md` - Board support matrix
- `BOARDS.md` - This file

### 4. Test Build

```bash
platformio run -e my-new-board
```

### 5. Submit PR

Include:
- Hardware details
- Build logs
- Test results
- Photos/screenshots

---

## Troubleshooting

### Build Errors

**Problem:** Unknown board ID
- Check PlatformIO's board list: `platformio boards espressif32`
- Use a similar board as template

**Problem:** USB CDC errors
- S2/S3 need `-DARDUINO_USB_CDC_ON_BOOT=1`
- S3 also needs `-DARDUINO_USB_MODE=1`

**Problem:** Upload fails
- Try adding upload flags:
  ```ini
  upload_flags = 
      --before=no_reset
      --after=hard_reset
  ```

### Runtime Issues

**Problem:** Board boots but no WiFi
- Check WiFi credentials in `config/wifi_credentials.h`
- Board will create AP if credentials missing

**Problem:** HID doesn't work
- Verify board has native USB (S2/S3 only)
- Check USB cable supports data (not just power)
- Try different USB port

**Problem:** MCP connection fails
- Check board IP: `platformio device monitor`
- Verify WebSocket port 8080 is accessible
- Check firewall settings

---

## Future Expansion

Planned features for board support:

### BLE HID
- Enable keyboard/mouse over BLE for non-USB boards
- Support for ESP32 Classic, C3, C6

### OTA Updates
- Over-the-air firmware updates
- No need to connect via USB

### Display Support
- Show status on T-Display-S3
- Visual HID command feedback

### Battery Optimization  
- Low-power modes for Feather boards
- Battery level monitoring

### Multi-Device
- Control multiple target devices
- Device switching commands

---

## Board Purchase Links

> Note: These are not affiliate links, just references

### ESP32-S2
- [ESP32-S2 Saola-1](https://www.espressif.com/en/products/devkits)
- [Adafruit Feather ESP32-S2](https://www.adafruit.com/product/5000)
- [Unexpected Maker FeatherS2](https://unexpectedmaker.com/shop/feathers2-esp32-s2)

### ESP32-S3
- [ESP32-S3 DevKitC-1](https://www.espressif.com/en/products/devkits)
- [Adafruit Feather ESP32-S3](https://www.adafruit.com/product/5477)
- [LilyGo T-Display-S3](http://www.lilygo.cn/)

### ESP32 Classic & C3/C6
- Generic boards available on Amazon, AliExpress, etc.

---

## Build Status

![Build Status](https://github.com/HowieDuhzit/psai-ducky/actions/workflows/build.yml/badge.svg?branch=dev)

All board builds are automatically tested on every commit via GitHub Actions.
