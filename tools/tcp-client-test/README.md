# Docker TCP Client for Example 6 (Zephyr TCP Server)

This Docker module provides a TCP client for testing the **Example 6 TCP Server** running on a Zephyr board.

## Purpose

The client simulates a remote host connecting to the Zephyr TCP server and sends test messages that should be echoed back. This allows you to verify the server implementation without needing additional hardware or manual testing.

## Quick Start

### Prerequisites

Before running this client, ensure:
1. **Example 6 firmware** is flashed on your Zephyr board
2. **Board is powered on** and connected to your network
3. **Docker and Docker Compose** are installed on your host machine

### Step 1: Build and Run the Docker Client

```bash
# Navigate to the docker-tcp-client directory
cd apps/tools/docker-tcp-client

# Build and run the container
docker-compose up --build
```

### Step 2: Monitor Board Serial Output

In another terminal, open a serial monitor to see the board receiving connections:

```bash
# Option 1: Using VS Code task
# Run "Serial Monitor" from Command Palette (Ctrl+Shift+P)

# Option 2: Using miniterm directly
python -m serial.tools.miniterm /dev/ttyUSB0 115200 --eol LF
# On Windows: COM3 instead of /dev/ttyUSB0
```

### Step 3: Verify Output

**Expected Docker client output:**
```
[INFO] TCP Client for Zephyr Server
[INFO] Connecting to 192.168.1.100:5555...

[CONNECTED] Successfully connected to 192.168.1.100:5555

[1] Sending: Hello from Docker!
[1] Received: Hello from Docker!
[1] ✓ Echo verified!

[2] Sending: Test message 1
[2] Received: Test message 1
[2] ✓ Echo verified!

...

[INFO] Connection closed
[INFO] Test completed successfully!
```

**Expected board serial output:**
```
[tcp-server] IPv4 address assigned: 192.168.1.100/24, gateway: 192.168.1.1, dns: 192.168.1.1
[tcp-server] TCP Server listening on 0.0.0.0:5555...
[tcp-server] Client 192.168.1.x:xxxxx connected
[tcp-server] Received (19 bytes): Hello from Docker!
[tcp-server] Sending echo: Hello from Docker!
[tcp-server] Received (15 bytes): Test message 1
[tcp-server] Sending echo: Test message 1
...
[tcp-server] Client disconnected
```

## How It Works

### Network Architecture

```
┌─────────────────────────────────────────────────────┐
│                   Host Machine                       │
│  ┌──────────────────────────────────────────────┐   │
│  │  Docker Container (tcp_client.py)            │   │
│  │  - Connects to 192.168.1.100:5555           │   │
│  │  - Sends test messages                       │   │
│  │  - Receives echoed responses                 │   │
│  └──────┬───────────────────────────────────────┘   │
│         │ (eth_network bridge)                       │
│  ┌──────┴───────────────────────────────────────┐   │
│  │  Docker Network (eth_network)                │   │
│  │  - Bridges docker-compose to host network    │   │
│  └──────┬───────────────────────────────────────┘   │
│         │ (Physical or Virtual Ethernet)             │
└────────┬──────────────────────────────────────────────┘
         │
    ┌────┴──────────────────────────────────────┐
    │   Zephyr Board (STM32H573I-DK)            │
    │   - Runs Example 6 TCP Server             │
    │   - Listening on 192.168.1.100:5555       │
    │   - Echoes received messages              │
    └──────────────────────────────────────────┘
```

### Connection Flow

1. **Client Initialization**: tcp_client.py starts and creates a TCP socket
2. **Connection**: Connects to board at 192.168.1.100:5555
3. **Message Exchange Loop**:
   - Client sends a test message
   - Server receives and echoes back
   - Client receives the echo
   - Verify echo matches original message
4. **Cleanup**: Client closes socket and Docker container exits


## Author Notes

- Client sends 4 different test messages
- Verifies each echo matches the original message
- Gracefully handles connection errors
- Suitable for both automated testing and manual verification
- Easily extensible for custom test scenarios
