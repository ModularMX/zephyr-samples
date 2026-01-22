# UDP Receiver Test

Simple Python UDP server for testing the UDP sender example.

## Usage

```bash
python udp_receiver_test.py
```

The script will:
- Listen on `0.0.0.0:4242` (all interfaces)
- Wait for UDP packets
- Display each packet received with sender IP, port, and data

## Expected Output

```
[INFO] Starting UDP server on 0.0.0.0:4242
[OK] Bound to 0.0.0.0:4242
[INFO] Listening... (Ctrl+C to stop)
[RX] From 192.168.1.100:51810 - 34 bytes
     Data: Hello from Zephyr UDP! [packet #0]
[RX] From 192.168.1.100:51810 - 34 bytes
     Data: Hello from Zephyr UDP! [packet #1]
```

## Troubleshooting

### Port already in use
```bash
netstat -ano | findstr 4242
taskkill /PID <PID> /F
```

### No packets received
- Check STM32 is running UDP sender example
- Verify Ethernet cable connection
- Check firewall allows UDP port 4242
- Confirm IP addresses: STM32=192.168.1.100, Host=192.168.1.1

## Why not Docker?

Docker Desktop on Windows has limitations with UDP port mapping. This simple Python script is more reliable.
