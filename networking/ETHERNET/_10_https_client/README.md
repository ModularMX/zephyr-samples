# HTTPS Client Example

This is an HTTPS client example based on the HTTP client example (example 09) with TLS/HTTPS support.

## Overview

This example demonstrates how to use the Zephyr HTTP client to make HTTPS requests to a server using TLS encryption. It includes:

- **Static IP Configuration**: Assigns a static IP address (192.168.1.100/24)
- **Network Connectivity**: Waits for network connectivity before making requests
- **TLS/HTTPS Support**: Uses Mbed TLS for secure connections on port 4443
- **Multiple Requests**: Demonstrates both GET and POST requests over HTTPS

## Key Features

- Uses the same CA certificate as Zephyr's http_client sample
- Simplified TLS socket setup and connection management
- HTTP GET and POST requests via HTTPS
- Network event monitoring and IP address display

## Configuration

### Server Settings
- **Server IP**: 192.168.1.1 (configurable in src/main.c)
- **Port**: 4443 (HTTPS standard port)
- **Protocol**: HTTPS/TLS 1.2

### Network Settings
- **Device IP**: 192.168.1.100/24
- **Netmask**: 255.255.255.0
- **Gateway**: 192.168.1.1

## Building and Running

### Prerequisites
1. Zephyr development environment set up
2. ESP32C6 or similar board with Ethernet support
3. A server listening on 192.168.1.1:4443 with a valid HTTPS certificate

### Build
```bash
cd apps/networking/ETHERNET/_10_https_client
west build -b esp32c6_devkitc
```

### Flash
```bash
west flash
```

### Monitor Serial Output
```bash
python -m serial.tools.miniterm /dev/ttyUSB0 115200 --eol LF
```

## Expected Output

```
--- Zephyr HTTPS Client Example ---
[HTTPS] CA certificate registered
Connecting to 192.168.1.1:4443
[HTTPS] Sending GET request...
[HTTPS] Status: HTTP/1.1 200 OK
[HTTPS] All data received (123 bytes)
[HTTPS] Body fragment: ...
[HTTPS] Sending POST request...
[HTTPS] Status: HTTP/1.1 200 OK
[HTTPS] All data received (45 bytes)
[HTTPS] Done.
```

## SSL/TLS Certificate

The example uses the `https-cert.der` certificate file, which is the same certificate used in Zephyr's http_client sample located at:
`zephyr/samples/net/sockets/http_client/src/https-cert.der`

This certificate is automatically included during the build process via the CMakeLists.txt file, which generates the `https-cert.der.inc` header file.

## Customization

To connect to a different server:

1. Modify `SERVER_IP` and `SERVER_PORT` in `src/main.c`
2. Update `TLS_PEER_HOSTNAME` in `src/ca_certificate.h` to match your server's hostname
3. Replace `https-cert.der` with your server's certificate if needed

## Testing with a Local HTTPS Server

You can test this example using the net-tools server that comes with Zephyr:

```bash
cd zephyr/samples/net/sockets/http_client
./../../common/net-tools/https-server.sh
```

This will start an HTTPS server on 192.168.1.1:4443 using the test certificate.

## References

- [Zephyr HTTP Client Sample](../../../zephyr/samples/net/sockets/http_client/)
- [Zephyr TLS Credentials](https://docs.zephyrproject.org/latest/reference/networking/tls.html)
- [Zephyr Socket API](https://docs.zephyrproject.org/latest/reference/networking/socket.html)
