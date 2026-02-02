# HTTP Server Example - Hello World

A beginner-friendly HTTP server example for Zephyr RTOS running on STM32H573I-DK with Ethernet connectivity.

## What It Does

This example implements a simple HTTP server that:
- Serves a web interface with device information display
- Provides a JSON endpoint with device details (board, architecture, uptime)
- Includes an echo endpoint for testing POST requests
- Updates device status in real-time via JavaScript polling

## Features

- **Static Web Interface**: Beautiful responsive HTML/CSS UI
- **Device Info Endpoint**: GET `/device-info` returns JSON with device data
- **Echo Service**: POST `/echo` echoes back received data
- **Real-time Updates**: JavaScript fetches device info every 2 seconds
- **Compressed Resources**: HTML and JS files are gzip-compressed at build time

## Building

```bash
west build -b stm32h573i_dk apps/networking/ETHERNET/_11_http_server_basic
```

## Flashing

```bash
west flash
```

Or use the Flash task in VS Code.

## Usage

1. Power on the device with Ethernet connected
2. Open a browser and navigate to: `http://192.168.1.100:8080`
3. You should see the "Hello World" interface with device info

## Available Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/` | Main HTML page |
| GET | `/main.js` | JavaScript file |
| GET | `/device-info` | JSON with device information |
| GET/POST | `/echo` | Echo service (echoes back request body) |

## Testing with curl

**Get device info:**
```bash
curl http://192.168.1.100:8080/device-info
```

Expected response:
```json
{"board":"STM32H573I-DK","arch":"ARM Cortex-M33","uptime":12345,"status":"Running"}
```

**Send echo request:**
```bash
curl -X POST -d "Hello Device" http://192.168.1.100:8080/echo
```

Response: `Hello Device`

## Network Configuration

- **IP Address**: 192.168.1.100
- **Gateway**: 192.168.1.1
- **Subnet**: 255.255.255.0
- **Port**: 8080

Ensure your device is on the same network segment (192.168.1.x).

## Serial Output

Monitor the device with:
```bash
python -m serial.tools.miniterm COM<N> 115200
```

You should see messages like:
```
[device] Starting HTTP Server on port 8080
[device] Available endpoints:
[device]   GET  /              -> HTML page
[device]   GET  /main.js       -> JavaScript
[device]   GET  /device-info   -> Device information (JSON)
[device]   GET/POST /echo      -> Echo server
```

## Project Structure

```
_11_http_server_basic/
├── src/
│   ├── main.c                          # HTTP server implementation
│   └── static_web_resources/
│       ├── index.html                  # Web interface (compressed)
│       └── main.js                     # Client-side logic (compressed)
├── CMakeLists.txt                      # Build configuration
├── Kconfig                             # Kernel options
├── prj.conf                            # Zephyr configuration
└── sections-rom.ld                     # Linker script for resources
```

## Learning Points

This example demonstrates:
- HTTP server configuration in Zephyr
- Static resource serving with gzip compression
- Dynamic endpoint handling with JSON responses
- Real-time device monitoring via web interface
- Client-server communication patterns

