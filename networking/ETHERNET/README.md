# Ethernet Networking Examples

Basic Zephyr Ethernet networking examples for STM32H573I-DK and compatible boards.

## Examples

### _01_basic_connection
Establishes basic Ethernet connection and logs status.

### _02_basic_connection_cb
Ethernet connection with event callbacks.

### _03_static_ip
Static IP address configuration.

### _04_dhcp
Dynamic IP configuration with DHCP.

### _05_tcp_client
TCP client that connects to a server and sends/receives data.

**Use cases:**
- Remote device communication
- Cloud connectivity
- Command & control

### _06_tcp_server
TCP server that accepts connections and handles multiple clients.

**Use cases:**
- Device control interface
- Data collection hub
- Configuration server

### _07_udp_sender
UDP sender that transmits packets to a server.

**Use cases:**
- Telemetry data transmission
- Sensor readings to cloud
- Status updates to server

### _08_udp_receiver
UDP receiver that listens for incoming packets.

**Use cases:**
- Remote device commands
- Configuration updates
- Data collection from multiple sources

### _09_http_client
HTTP client that makes GET and POST requests to an HTTP server.

**Use cases:**
- REST API communication
- Cloud connectivity
- Data upload/download
- Configuration retrieval

### _10_https_client
HTTPS client that makes secure GET and POST requests using TLS/SSL encryption.

**Use cases:**
- Secure REST API communication
- Encrypted cloud connectivity
- Secure data transmission
- Authenticated API calls

**Features:**
- TLS 1.2 encryption using Mbed TLS
- Certificate-based server verification
- Support for both GET and POST requests
- JSON payload support
- Server hostname validation

## Network Configuration

Default: 192.168.1.100/24

Each example has a README with build, flash, and test instructions.


> **Warning**
> These are basic examples. For production, add:
> - Thread management
> - Error recovery and retry logic
> - Proper timeout handling
> - Packet validation and checksums
> - Encryption/authentication
> - Connection state management
> - Memory leak prevention
> - Watchdog timers
> - Comprehensive logging
> - Rate limiting
> - Fragmentation handling

## References

- [Zephyr Networking Docs](https://docs.zephyrproject.org/latest/connectivity/networking/index.html)
