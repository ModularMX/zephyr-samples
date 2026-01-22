# UDP Receiver Example

Simple UDP receiver that listens for incoming packets with static IP configuration. This is the counterpart to the UDP Sender example (_07_udp_sender).

## Features

- Static IP address configuration
- UDP socket creation with bind()
- Receive incoming datagrams using recvfrom()
- Display sender information (IP, port, data)
- Board-agnostic implementation

## Architecture

### Comparison with Sender

| Aspect | Sender (_07) | Receiver (_08) |
|--------|--------------|----------------|
| Socket Operation | `connect()` → `send()` | `bind()` → `recvfrom()` |
| Board IP | 192.168.1.100 | 192.168.1.200 |
| Role | Sends packets | Receives packets |
| Target | External server | Local port 4242 |

## Configuration

Edit `src/main.c` to customize:

```c
#define LOCAL_PORT 4242                    // UDP port to bind to
#define BOARD_IP htonl(...)                // Your board's static IP (192.168.1.200)
#define BOARD_IP_MASK 24                   // Netmask: /24 (255.255.255.0)
#define BUFFER_SIZE 256                    // Max datagram size
```

## Usage

### Build
```bash
west build -b stm32h573i_dk apps/networking/ETHERNET/_08_udp_receiver
```

### Flash
```bash
west flash
```

## Testing

### Test with Sender Example
1. Flash receiver to one board (or run in Docker)
2. Flash sender to another board (or use Python sender)
3. Both send and receive on 192.168.1.1 network
4. Monitor serial console to see packets arriving


### Expected Output (Serial Console)
```
[eth_udp_receiver] =
== UDP Receiver ===
[eth_udp_receiver] Creating UDP socket...
[eth_udp_receiver] Socket created
[eth_udp_receiver] Binding to port 4242...
[eth_udp_receiver] Socket bound to 0.0.0.0:4242
[eth_udp_receiver] Waiting for UDP packets... (Ctrl+C to stop)
[eth_udp_receiver] Packet #1 received from 192.168.1.100:12345 (34 bytes)
[eth_udp_receiver]   Data: 'Hello from Zephyr UDP! [packet #0]'
```

## Key Code Patterns

### Socket Creation
```c
sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
```

### Bind to Local Port
```c
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(LOCAL_PORT);
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

### Receive Packets
```c
int received = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                        (struct sockaddr *)&client_addr, &client_addr_len);
```

## Networking Notes

- Receiver binds to `0.0.0.0:4242` (all interfaces)
- Sender connects to specific IP:port before sending
- Both use static IP configuration via `net_if_ipv4_addr_add()`
- Network event callbacks notify when IP is assigned


## Notes

- UDP is connectionless (no handshake required)
- No guaranteed delivery (packets may be lost)
- Lower latency than TCP
- Good for streaming, telemetry, and real-time applications
