
# Python TCP Client for Example 6 (Zephyr TCP Server)

This module provides a Python TCP client for testing the **Example 6 TCP Server** running on a Zephyr board.


## Purpose

The client simulates a remote host connecting to the Zephyr TCP server and sends test messages that should be echoed back. This allows you to verify the server implementation without additional hardware or manual testing.


## Quick Start

### Prerequisites

Before running this client, ensure:
1. **Example 6 firmware** is flashed on your Zephyr board
2. **Board is powered on** and connected to your network
3. **Python 3** is installed on your host machine

### 1. Run the TCP Client

```bash
# Navigate to the tcp-client-test directory
cd apps/tools/tcp-client-test

# Run the script directly with Python
python tcp_client.py
```

### 2. Monitor Board Serial Output

In another terminal, open a serial monitor to see the board receiving connections:

```bash
# Option 1: Using VS Code task
# Run "Serial Monitor" from Command Palette (Ctrl+Shift+P)

# Option 2: Using miniterm directly
python -m serial.tools.miniterm COM3 115200 --eol LF
# On Linux/Mac: use /dev/ttyUSB0 instead of COM3
```

### 3. Verify Output


**Expected TCP client output:**
```
[INFO] TCP Client for Zephyr Server
[INFO] Connecting to 192.168.1.100:5555...

[CONNECTED] Successfully connected to 192.168.1.100:5555

[1] Sending: Hello from TCP client!
[1] Received: Hello from TCP client!
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
[tcp-server] Received (19 bytes): Hello from TCP client!
[tcp-server] Sending echo: Hello from TCP client!
[tcp-server] Received (15 bytes): Test message 1
[tcp-server] Sending echo: Test message 1
...
[tcp-server] Client disconnected
```


## How It Works

### Network Architecture

```
┌──────────────────────────────────────────────┐
│           Host PC (tcp_client.py)            │
│  - Connects to 192.168.1.100:5555           │
│  - Sends test messages                      │
│  - Receives echoed responses                │
└──────────────┬──────────────────────────────┘
               │ (Ethernet/WiFi)
    ┌──────────┴──────────────────────────────┐
    │   Zephyr Board (STM32H573I-DK)         │
    │   - Runs Example 6 TCP Server          │
    │   - Listens on 192.168.1.100:5555      │
    │   - Echoes received messages           │
    └───────────────────────────────────────┘
```

### Connection Flow

1. **Client Initialization**: tcp_client.py starts and creates a TCP socket
2. **Connection**: Connects to board at 192.168.1.100:5555
3. **Message Exchange Loop**:
   - Client sends a test message
   - Server receives and echoes back
   - Client receives the echo
   - Verifies echo matches original message
4. **Cleanup**: Client closes socket and exits



## Notes

- Client sends 4 different test messages
- Verifies each echo matches the original message
- Gracefully handles connection errors
- Suitable for both automated testing and manual verification
- Easily extensible for custom test scenarios
