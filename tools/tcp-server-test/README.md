
# Python TCP Echo Server

Standalone Python module for testing TCP communication.


## Quick Start

```bash
# Navigate to the tcp-server-test directory
cd apps/tools/tcp-server-test

# Run the echo server directly with Python
python tcp_echo_server.py
```

Server will listen on:
- IP: 0.0.0.0 (all interfaces)
- Port: 4242
- Protocol: TCP


## What It Does

- Python TCP echo server (no Docker required)
- No manual setup needed - just run!


## Files

- `tcp_echo_server.py` - Echo server implementation


## Network

The server listens on all interfaces (0.0.0.0) by default:
- Server IP: 0.0.0.0 (use your PC's IP on the LAN)
- Port: 4242


## Commands

```bash
# Start server
python tcp_echo_server.py

# Stop server
Ctrl+C
```


## Requirements

- Python 3 installed


## Usage

1. Start the server from this folder
2. Flash the board with Example 5 firmware
3. Board will connect to your PC's IP address on port 4242
4. Watch the echo server log for connections
