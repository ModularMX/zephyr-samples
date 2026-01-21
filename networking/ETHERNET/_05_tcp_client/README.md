# Example 5: Generic TCP Client with Static IP

## Overview

This example demonstrates a **generic TCP Client** implementation compatible with any board:
1. Assigns a static IPv4 address (configurable)
2. Creates a TCP socket (AF_INET, SOCK_STREAM)
3. Connects to a TCP server (configurable IP:port)
4. Sends data (configurable message)
5. Receives the response
6. Closes the connection gracefully

This is a **didactic, reusable example** showing every step of TCP client communication - works with any Zephyr-supported board.

## Quick Start

### Prerequisites
- Docker & Docker-compose

### Steps

**1. Start the Docker echo server (Terminal 1):**
```bash
cd apps/tools/docker-tcp-echo
docker-compose up
```
Wait for: `[INFO] Listening on 0.0.0.0:4242`

**2. Flash the board (Terminal 2):**
```bash
west flash -b <BOARD_NAME> apps/networking/ETHERNET/_05_TCP_client
```
Replace `<BOARD_NAME>` with your board (e.g., `stm32h573i_dk`, `nrf52840dk_nrf52840`, `qemu_x86`, etc.)

**3. Monitor serial output (Terminal 3):**
```bash
python -m serial.tools.miniterm COM3 115200 --eol LF
```
*(Replace COM3 with your board's serial port)*

**Expected board output:**
```
[00:00:00.xxx,000] <inf> eth_tcp_client: TCP Client with Static IP
[00:00:00.xxx,000] <inf> eth_tcp_client: Network interface found
[00:00:01.xxx,000] <inf> eth_tcp_client: Static IP assigned: 192.168.1.100/24
[00:00:02.xxx,000] <inf> eth_tcp_client: Ready to connect to TCP server!
[00:00:02.xxx,000] <inf> eth_tcp_client: Creating TCP socket...
[00:00:02.xxx,000] <inf> eth_tcp_client: Socket created successfully
[00:00:02.xxx,000] <inf> eth_tcp_client: Connecting to server...
[00:00:02.xxx,000] <inf> eth_tcp_client: Connected to server!
[00:00:02.xxx,000] <inf> eth_tcp_client: Sending: 'Hello from Zephyr!'
[00:00:02.xxx,000] <inf> eth_tcp_client: Data sent (18 bytes)
[00:00:02.xxx,000] <inf> eth_tcp_client: Waiting for response...
[00:00:02.xxx,000] <inf> eth_tcp_client: Received (18 bytes): 'Hello from Zephyr!'
[00:00:02.xxx,000] <inf> eth_tcp_client: Connection closed
```

**Expected Docker output:**
```
[CONNECTED] Client connected from 172.18.0.1:xxxxx
[RECEIVED] (18 bytes): 'Hello from Zephyr!'
[SENDING] Echoing back: 'Hello from Zephyr!'
[SENT] Echo sent successfully
[DISCONNECTED] Client disconnected
```

## Network Architecture

**Zephyr Board** ←→ **Docker Container** (TCP Echo Server)
- **Board IP**: 192.168.1.100/24 (static, assigned by firmware)
- **Server IP**: 172.18.0.1 (Docker bridge network, configurable)
- **Server Port**: 4242 (configurable)
- **Protocol**: TCP IPv4

Network configuration is managed by docker-compose and configurable in firmware.

## Code Structure

### Key Functions

**`assign_static_ip(struct net_if *iface)`**
- Assigns static IPv4 192.168.1.100/24 to the network interface
- Uses `net_if_ipv4_addr_add()` with NET_ADDR_MANUAL type

**`ip_event_handler(...)`**
- Callback triggered by NET_EVENT_IPV4_ADDR_ADD
- Displays assigned IP address, netmask, and gateway

**`tcp_connect_and_send(void)`** - Complete TCP client lifecycle

```c
// 1. Create socket
tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

// 2. Prepare server address
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(SERVER_PORT);
inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

// 3. Connect to server
connect(tcp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

// 4. Send data
send(tcp_socket, SEND_DATA, sizeof(SEND_DATA) - 1, 0);

// 5. Receive response
recv(tcp_socket, recv_buffer, RECV_BUF_SIZE - 1, 0);

// 6. Close socket
close(tcp_socket);
```

### Main Flow (Execution Order)

1. **Boot** → Get default network interface
2. **IP Assignment** → Assign static IP 192.168.1.100/24
3. **IP Event** → Wait for NET_EVENT_IPV4_ADDR_ADD callback
4. **Network Stability** → Sleep 1 second
5. **Socket Creation** → `socket(AF_INET, SOCK_STREAM, 0)`
6. **Connection** → `connect()` to 192.168.1.1:4242
7. **Data Exchange** → `send()` → `recv()`
8. **Cleanup** → `close()` and exit


## Docker Echo Server

The TCP echo server is a **separate, reusable module** in:
```
apps/tools/docker-tcp-echo/
```

**Features:**
- Python 3.11-slim container
- Listens on 0.0.0.0:4242
- Receives data and echoes it back immediately
- Disconnects after each message (didactic behavior)
- Logs all connections and data transfers

**Can be reused for:**
- Testing any TCP client implementation
- Understanding TCP protocol flow
- Debugging network connectivity issues

## See Also

- **Example 4**: DHCP client (automatic IP)
- **Example 3**: Static IP (no networking)
- **Example 2**: Link detection
- **Example 1**: Basic Ethernet
