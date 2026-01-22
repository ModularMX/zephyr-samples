# UDP Receiver Example

Simple UDP receiver that listens for incoming packets with static IP configuration.

## Features

- Static IP address configuration
- UDP socket with `bind()` and `recvfrom()`
- Display sender information (IP, port, data)
- Waits for network connection before listening

## Configuration

Edit `src/main.c`:

```c
#define LOCAL_PORT 4242                    // UDP port to listen on
#define BOARD_IP_ADDR htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL)
#define BOARD_IP_MASK 24
```

## Build & Flash

```bash
west build -b stm32h573i_dk apps/networking/ETHERNET/_08_udp_receiver
west flash
```

## Testing with Python

**Terminal 1 - Serial monitor:**
```bash
python -m serial.tools.miniterm COM3 115200
```

**Terminal 2 - Send UDP packets:**
```bash
python apps/tools/udp-sender-test/udp_sender_test.py
```

**Expected output:**
```
[eth_udp_receiver] Received 34 bytes from 192.168.1.1:12345
[eth_udp_receiver]   Data: Hello from Python UDP! [packet #0]
```

## References

- [Zephyr Networking](https://docs.zephyrproject.org/latest/connectivity/networking/index.html)

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
