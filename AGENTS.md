# Repository Guidelines

## Project Structure & Module Organization
PlatformIO firmware lives in `src/` (`main.cpp` bootstraps setup/loop, `mcp_server.cpp` handles JSON-RPC, `wifi_manager.cpp` manages STA/AP, `hid_controller.cpp` wraps HID) with matching headers under `include/`. The Node.js MCP bridge and desktop helpers stay at the repository root (`index.js`, `test.js`, `final-demo.py`, `monitor-hid.py`). Hardware guides, flashing aides, and reference binaries are in `docs/`, `flipper_flash/`, and alongside `FLASH_INSTRUCTIONS.md`; experimental sketches belong in `examples/`. Keep environment-specific headers in `config/` and add new sensitive files to `.gitignore`.

## Build, Test, and Development Commands
- `platformio run -e development` builds the firmware with debug defines for ESP32-S3.
- `platformio run -e esp32-s3-devkitc-1 -t upload` compiles and flashes over USB.
- `npm start` (alias `node index.js`) launches the MCP bridge using `ESP32_HOST`/`ESP32_PORT`.
- `npm test` exercises initialization, tool listing, and keyboard flows through the Node harness.
- `python3 test_client.py --host 192.168.4.1` probes the firmware while it advertises its AP.

## Coding Style & Naming Conventions
Use 4-space indentation in Arduino and Node files. Declare classes in `include/`, implement in `src/`, and stick to `PascalCase` for types, `camelCase` for methods/variables, and `SCREAMING_SNAKE_CASE` for constants or macros. Keep JSON keys lower_snake_case to match the MCP schema and prefer `const`/`let` plus async/await with early returns in JavaScript. Add brief comments only for non-obvious hardware interactions.

## Testing Guidelines
Validate firmware edits with a board connected over USB while `npm test` runs so JSON-RPC commands reach the target. Follow up with `python3 test_client.py` when checking AP-mode fallbacks. Name new helper scripts `test-<feature>.py` or `test_<feature>.js`, document prerequisites near the top, and keep smoke tests passing on both `development` and `esp32-s3-devkitc-1` environments before opening a PR.

## Commit & Pull Request Guidelines
There is no project history yet, so seed it with short imperative subject lines (e.g., `Add OTA fallback for WiFi reconnect`). Group related firmware and bridge updates, describe the board, firmware environment, and commands executed inside the commit or PR body, and cross-link issue IDs (`Fixes #12`) when applicable. Pull requests should list the test matrix and attach logs or screenshots for HID-facing changes.

## Security & Configuration Tips
Never commit real Wi-Fi credentials or API keys. Keep placeholders in `include/config.h`, override targets through environment variables (`ESP32_HOST`, `ESP32_PORT`), or store local-only headers under `config/` that remain ignored. Review `MAX_CLIENTS`, authentication flags, and timeout values before sharing binaries or demos.
