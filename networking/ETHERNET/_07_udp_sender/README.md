# UDP Sender Example

Simple UDP sender that transmits packets to a server with static IP configuration.

## Features

- Static IP address configuration
- Simple UDP socket creation
- Periodic packet sending
- Board-agnostic implementation

## Configuration

Edit `src/main.c` to customize:

```c
#define SERVER_ADDR "192.168.1.1"      // Server IP (your PC)
#define SERVER_PORT 4242               // Server UDP port
#define BOARD_IP_ADDR ...              // Your board's static IP
#define SEND_INTERVAL_MS 2000          // Send interval in milliseconds
```

## Usage

### Build
```bash
west build -b stm32h573i_dk apps/networking/ETHERNET/_07_udp_sender
```

### Flash
```bash
west flash
```

## Testing on PC

### Using the provided Python host test script


You can use the provided Python script to receive UDP packets on your PC:

```bash
cd apps/networking/ETHERNET/_07_udp_sender/pc_receiver
python udp_receiver_test.py
```

This script will display incoming UDP packets from the Zephyr board.

Alternatively, you can use netcat (Linux/Mac):
```bash
nc -lu 0.0.0.0 4242
```

### Windows (using netcat)
```bash
nc -u -l -p 4242
```

Or use a Python UDP receiver:
```bash
python3 -c "
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(('0.0.0.0', 4242))
while True:
    data, addr = s.recvfrom(1024)
    print(f'From {addr}: {data.decode()}')
"
```

## Notes

- UDP is connectionless (no handshake required)
- No guaranteed delivery (packets may be lost)
- Lower latency than TCP
- Good for streaming, telemetry, and real-time applications
