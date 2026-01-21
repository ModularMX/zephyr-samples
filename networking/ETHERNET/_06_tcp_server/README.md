# Example 6: Generic TCP Server with Static IP (Simple Sequential)

## Overview

This example demonstrates a **simple TCP Server** implementation compatible with any board:
1. Assigns a static IPv4 address (configurable)
2. Creates a TCP socket
3. Binds to a listening port (configurable)
4. Accepts one client at a time
5. Echoes received data back to the client
6. Disconnects and waits for the next client

This is a **didactic, sequential server** showing every step of TCP server communication - works with any Zephyr-supported board.

**Note:** This is a simple single-threaded implementation. Each client is handled sequentially (one at a time). For production with multiple concurrent clients, see Example 6b (Advanced with threads).

## Quick Start

### Prerequisites
- Docker Desktop: https://www.docker.com/products/docker-desktop
- For testing: Example 5 (TCP Client) on another board, or a TCP client running on your PC

### Steps

**1. Build and Flash the TCP Server (Terminal 1):**
```bash
west flash -b <BOARD_NAME> apps/networking/ETHERNET/_06_TCP_client
```
Replace `<BOARD_NAME>` with your board (e.g., `stm32h573i_dk`, `nrf52840dk_nrf52840`, `qemu_x86`, etc.)

**2. Monitor serial output (Terminal 2):**
```bash
python -m serial.tools.miniterm COM3 115200 --eol LF
```
*(Replace COM3 with your board's serial port)*

**3. Run Docker TCP Client (Terminal 3):**

The Docker TCP client simulates a remote host connecting to your server and sending test messages.

```bash
cd apps/tools/docker-tcp-client
docker-compose up --build
```

This will:
- Build the Docker image (first time only)
- Connect to your board at 192.168.1.100:5555
- Send 4 test messages
- Verify each echo matches the original
- Display results and exit

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

[3] Sending: Test message 2
[3] Received: Test message 2
[3] ✓ Echo verified!

[4] Sending: Zephyr TCP Echo
[4] Received: Zephyr TCP Echo
[4] ✓ Echo verified!

[INFO] Connection closed
[INFO] Test completed successfully!
```

**Expected board output:**
```
[00:00:00.xxx,000] <inf> eth_tcp_server: TCP Server with Static IP
[00:00:00.xxx,000] <inf> eth_tcp_server: Network interface found
[00:00:01.xxx,000] <inf> eth_tcp_server: Static IP assigned: 192.168.1.100/24
[00:00:02.xxx,000] <inf> eth_tcp_server: Creating TCP server socket...
[00:00:02.xxx,000] <inf> eth_tcp_server: Server socket created successfully
[00:00:02.xxx,000] <inf> eth_tcp_server: Binding to port 5555...
[00:00:02.xxx,000] <inf> eth_tcp_server: Socket bound successfully
[00:00:02.xxx,000] <inf> eth_tcp_server: Listening for incoming connections...
[00:00:02.xxx,000] <inf> eth_tcp_server: Server listening on 0.0.0.0:5555

[00:00:XX.xxx,000] <inf> eth_tcp_server: Client connected: 192.168.1.50:xxxxx
[00:00:XX.xxx,000] <inf> eth_tcp_server: Received (18 bytes): 'Hello from Zephyr!'
[00:00:XX.xxx,000] <inf> eth_tcp_server: Echoed (18 bytes) back to client
[00:00:XX.xxx,000] <inf> eth_tcp_server: Client disconnected
[00:00:XX.xxx,000] <inf> eth_tcp_server: Client socket closed
```

## Network Architecture

**TCP Client** ←→ **Zephyr Board (TCP Server)**
- **Server IP**: 192.168.1.100 (static, assigned by firmware)
- **Server Port**: 5555 (configurable)
- **Protocol**: TCP IPv4
- **Backlog**: 1 (one client at a time)

The server listens on all interfaces (0.0.0.0) but clients should connect to the board's static IP.

## Code Structure

### Key Functions

**`assign_static_ip(struct net_if *iface)`**
- Assigns static IPv4 192.168.1.100/24 to the network interface
- Uses `net_if_ipv4_addr_add()` with NET_ADDR_MANUAL type

**`ip_event_handler(...)`**
- Callback triggered by NET_EVENT_IPV4_ADDR_ADD
- Displays assigned IP address, netmask, and gateway

**`start_tcp_server(void)`** - TCP server main loop

```c
// 1. Create socket
server_socket = socket(AF_INET, SOCK_STREAM, 0);

// 2. Allow address reuse
setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, ...);

// 3. Bind to port
bind(server_socket, &server_addr, sizeof(server_addr));

// 4. Listen for connections
listen(server_socket, LISTEN_QUEUE_SIZE);

// 5. Accept clients loop
while (1) {
    client_socket = accept(server_socket, ...);
    handle_client(client_socket, ...);
}
```

**`handle_client(int client_socket, struct sockaddr_in *client_addr)`** - Echo handler

```c
// 6. Echo loop
while (1) {
    received = recv(client_socket, buffer, 1024, 0);
    if (received <= 0) break;
    send(client_socket, buffer, received, 0);  // Echo back
}

// 7. Close client
close(client_socket);
```

### Main Flow (Execution Order)

1. **Boot** → Get default network interface
2. **IP Assignment** → Assign static IP 192.168.1.100/24
3. **IP Event** → Wait for NET_EVENT_IPV4_ADDR_ADD callback
4. **Network Stability** → Sleep 1 second
5. **Socket Creation** → `socket(AF_INET, SOCK_STREAM, 0)`
6. **Bind** → `bind()` to port 5555
7. **Listen** → `listen()` for incoming connections
8. **Accept Loop** → `accept()` clients
9. **Echo** → Receive and echo back data

Each step is logged with `LOG_INF()` for visibility.

## Configuration (prj.conf)

```conf
CONFIG_NETWORKING=y              # Networking support
CONFIG_NET_SOCKETS=y             # Socket API
CONFIG_POSIX_API=y               # POSIX functions
CONFIG_REQUIRES_FULL_LIBC=y      # Full C library
CONFIG_NET_TCP=y                 # TCP protocol
CONFIG_NET_L2_ETHERNET=y         # Ethernet driver
CONFIG_MAIN_STACK_SIZE=3072      # Stack size for sockets
```

## Customization

All parameters are easily configurable at the top of `src/main.c`:

```c
// ============================================================================
// TCP SERVER CONFIGURATION - Customize these values for your setup
// ============================================================================
#define SERVER_PORT 5555                   // TCP server listening port
#define BACKLOG 1                          // Maximum clients in queue

// ============================================================================
// BOARD IP CONFIGURATION - Customize these values for your network
// ============================================================================
// IP Address (replace numbers to change: 192.168.1.100 → 10.0.0.50 etc)
#define BOARD_IP_ADDR htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL)
#define BOARD_IP_MASK 24                   // Netmask: /24 (255.255.255.0)
```

### Quick Customization Guide

**To change the server listening port:**
```c
#define SERVER_PORT 8080                   // Change from 5555 to 8080
```

**To change the board's IP address:**
Edit the `BOARD_IP_ADDR` define:
```c
// Current (192.168.1.100):
#define BOARD_IP_ADDR htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL)

// Example: Change to 10.0.0.50:
#define BOARD_IP_ADDR htonl((10UL << 24) | (0UL << 16) | (0UL << 8) | 50UL)
```

**To change the netmask (prefix length):**
```c
#define BOARD_IP_MASK 24                  // /24 = 255.255.255.0
#define BOARD_IP_MASK 16                  // /16 = 255.255.0.0
#define BOARD_IP_MASK 8                   // /8  = 255.0.0.0
```

### Rebuild and Flash

After customization:
```bash
west build -b <BOARD_NAME> apps/networking/ETHERNET/_06_TCP_server
west flash
```

## Files

- `src/main.c` - Server implementation
- `prj.conf` - Zephyr configuration
- `CMakeLists.txt` - Build config
- `boards/` - Board definitions


## Testing with Docker

### Why Use Docker for Testing?

Instead of needing a second Zephyr board or manually connecting with netcat/telnet, the Docker client automates the test process:

- **No extra hardware**: Test on the same machine as your development
- **Repeatable testing**: Same 4 messages every time
- **Automated verification**: Confirms echoes match original messages
- **Easy debugging**: Clear output showing exactly what was sent and received

### Docker Client Details

**Location**: `apps/tools/docker-tcp-client/`

**What it does**:
1. Connects to 192.168.1.100:5555 (your board's TCP server)
2. Sends 4 test messages in sequence
3. Receives echoed responses
4. Verifies each echo matches the original
5. Displays results and closes connection

**Requirements**:
- Docker and Docker Compose installed
- Board powered on and running Example 6 firmware
- Board reachable at 192.168.1.100 (adjust SERVER_IP in `tcp_client.py` if different)

### Customizing the Docker Client

**Change test messages**:

Edit `apps/tools/docker-tcp-client/tcp_client.py`:

```python
MESSAGES = [
    b'Your custom message 1',
    b'Your custom message 2',
    b'Test message 3',
]
```

**Connect to different board**:

If your board has a different IP, update `tcp_client.py`:

```python
SERVER_IP = '192.168.1.50'    # Change to your board's IP
SERVER_PORT = 5555             # Change port if needed
```

**Run without Docker** (direct Python):

```bash
cd apps/tools/docker-tcp-client
python3 tcp_client.py
```

### Troubleshooting Docker Client

**Connection refused:**
```
[ERROR] Connection refused!
```
- Verify board IP: check serial monitor for "Static IP assigned: 192.168.1.xxx"
- Update SERVER_IP in tcp_client.py to match
- Ensure board is powered on and firmware is running

**Connection timeout:**
```
[ERROR] Connection timeout!
```
- Board might not be responding at 192.168.1.100
- Try: `ping 192.168.1.100` from your host machine
- Check firewall isn't blocking the connection

**Container won't start:**
```bash
# Check logs
docker logs tcp-client

# Run interactively to see errors
docker-compose run --rm tcp-client
```

**Port conflicts:**
```bash
# Clean up existing containers
docker-compose down
docker system prune -a
```

## Limitations (Single-Threaded)

This implementation is **simple and sequential**:

**What it does:**
- Handles one client at a time
- Easy to understand
- Good for learning

**What it doesn't do:**
- Multiple concurrent clients
- Non-blocking I/O
- Production-level performance


## See Also

- **Example 5**: TCP Client (simple connection)
- **Example 4**: DHCP client (automatic IP)
- **Example 3**: Static IP (no networking)
- **Example 2**: Link detection
- **Example 1**: Basic Ethernet
