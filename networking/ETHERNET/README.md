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

### _11_http_server_basic
**Beginner-friendly HTTP server with web interface** for learning server-side networking.

**Use cases:**
- Device configuration web interface
- Real-time device monitoring
- REST API endpoint implementation
- Web-based data dashboard
- Learning HTTP protocol fundamentals

**Features:**
- Responsive web UI with HTML/CSS/JavaScript
- Device information endpoint (JSON)
- Echo service for POST testing
- Real-time device status updates
- Gzip-compressed static resources
- Multiple simultaneous connections
- Port: 8080

**Learning Topics:**
- HTTP server implementation in Zephyr
- Static resource serving
- Dynamic endpoint handlers
- JSON response formatting
- Client-server communication patterns

### _12_https_server_basic
**Beginner-friendly HTTPS server with TLS/SSL encryption** for secure web interface and REST API.

**Use cases:**
- Secure device configuration interface
- Encrypted REST API endpoints
- Production-ready server foundation
- Secure real-time monitoring
- Learning HTTPS and TLS fundamentals

**Features:**
- **Full TLS/SSL Encryption** - All traffic encrypted end-to-end
- **Auto-Generated Certificates** - Self-signed certificates generated automatically
- **Responsive Web UI** - Security indicators and HTTPS badges
- **Device Information Endpoint** - Secure JSON data over HTTPS
- **Echo Service** - Encrypted POST request testing
- **Stable Certificates** - Generated once, reused across builds (idempotent)
- **Static Resource Compression** - Gzip-compressed HTML and JavaScript
- **Multiple Connections** - TLS handshake with multiple simultaneous clients
- **Port: 4443** - Standard HTTPS port

**Certificate System:**
- Uses self-signed certificates (appropriate for embedded systems)
- Auto-generated with 2020-2100 validity window (no NTP/clock dependency)
- Certificates stored in `src/certs/` directory
- Regeneration prevented on each build (maintains stable HTTPS connections)
- Python script handles certificate lifecycle (idempotent generation)

**Learning Topics:**
- HTTPS/TLS protocol implementation
- SSL/TLS certificate generation and management
- TLS handshake process
- Encrypted communication channels
- Self-signed certificate usage
- Secure REST API design
- Production-ready server patterns

**Quick Start:**
```bash
# Build
west build -b stm32h573i_dk apps/networking/ETHERNET/_12_https_server_basic

# Flash
west flash

# Access (accept self-signed certificate warning)
https://192.168.1.100:4443
```

**Test with curl:**
```bash
curl -k https://192.168.1.100:4443/device-info
curl -k -X POST -d "test" https://192.168.1.100:4443/echo
```

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
