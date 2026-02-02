# HTTPS Server Example - Secure Hello World

A beginner-friendly HTTPS server example for Zephyr RTOS running on STM32H573I-DK with Ethernet connectivity and TLS/SSL encryption.

## What It Does

This example implements a secure HTTPS server that:
- Serves a web interface with device information display over encrypted connection
- Provides a JSON endpoint with device details (board, architecture, uptime)
- Includes an echo endpoint for testing POST requests with TLS protection
- Updates device status in real-time via JavaScript polling over secure HTTPS
- Uses self-signed certificates for TLS/SSL encryption
- Demonstrates TLS handshake and secure communication patterns

## Key Features

- **HTTPS Encryption**: All traffic is encrypted with TLS/SSL protocol
- **TLS Certificates**: Auto-generated self-signed certificates (stable across builds)
- **Static Web Interface**: Beautiful responsive HTML/CSS UI with security badges
- **Device Info Endpoint**: GET `/device-info` returns JSON over secure connection
- **Echo Service**: POST `/echo` echoes back data with TLS protection
- **Real-time Updates**: JavaScript fetches device info every 2 seconds via HTTPS
- **Compressed Resources**: HTML and JS files are gzip-compressed at build time

## SSL/TLS Certificate System

### Certificate Files

Two critical files enable secure HTTPS communication:

**`server_cert.der`** - Server Certificate
- Digital identity document presented to clients during HTTPS connection
- Contains:
  - Server identity (Subject: "STM32H573I-DK")
  - Public cryptographic key (clients use this to encrypt data)
  - Digital signature proving validity
  - Validity period (2020-2100 to avoid system clock issues)
- Format: DER (binary, space-efficient)

**`server_privkey.der`** - Server Private Key
- Secret key known only to the server
- Used for:
  - Decrypting data encrypted by clients with the public key
  - Digitally signing communications
  - Proving server authenticity
- **NEVER shared** - if compromised, security is broken
- Format: DER (binary)

### How TLS Works

```
1. Client connects to port 4443 (HTTPS)
2. Server sends: "Here's my certificate with my public key"
3. Client verifies certificate validity
4. Client generates encryption key, encrypts with public key, sends to server
5. Server decrypts using private key
6. Both now share encrypted communication channel
7. All subsequent data is encrypted both ways
```

### Certificate Generation Strategy

**Key Design: Certificates are generated ONCE and reused**

- First build: Python script `generate_certificates.py` creates `.der` files
- Subsequent builds: Script detects files exist, skips regeneration (idempotent)
- **Why?** Regenerating certificates each build would:
  - Break TLS handshake (certificate hash changes)
  - Cause "invalid certificate" errors
  - Prevent HTTPS connection

This ensures **stable, reliable HTTPS connections** across multiple builds.

### Self-Signed Certificates

These certificates are **self-signed** (not issued by Certificate Authority):
- ✅ Perfect for embedded devices and testing
- ✅ No external dependencies or internet needed
- ⚠️ Browsers will show security warning (expected and normal)
- ⚠️ Should NOT be used for production internet-facing services

## Building

```bash
# Clean build (will regenerate certificates if missing)
west build -p always -b stm32h573i_dk apps/networking/ETHERNET/_12_https_server_basic

# Regular build (reuses existing certificates)
west build -b stm32h573i_dk apps/networking/ETHERNET/_12_https_server_basic
```

## Flashing

```bash
west flash
```

Or use the Flash task in VS Code.

## Usage

1. Power on the device with Ethernet connected
2. Open a browser and navigate to: `https://192.168.1.100:4443`
3. Accept the security warning (self-signed certificate)
4. You should see the "Hello World" interface with HTTPS security badge
5. Connection is encrypted end-to-end

## Available Endpoints

| Method | Endpoint | Description | Security |
|--------|----------|-------------|----------|
| GET | `/` | Main HTML page | HTTPS/TLS |
| GET | `/main.js` | JavaScript file | HTTPS/TLS |
| GET | `/device-info` | JSON with device information | HTTPS/TLS |
| GET/POST | `/echo` | Echo service (echoes back request body) | HTTPS/TLS |

## Testing with curl

**Note:** Use `-k` flag to accept self-signed certificate

**Get device info securely:**
```bash
curl -k https://192.168.1.100:4443/device-info
```

Expected response:
```json
{"board":"STM32H573I-DK","arch":"ARM Cortex-M33","uptime":12345,"status":"Running"}
```

**Send secure echo request:**
```bash
curl -k -X POST -d "Hello Device" https://192.168.1.100:4443/echo
```

Response: `Hello Device` (transmitted over encrypted channel)

**Verify certificate details:**
```bash
openssl s_client -connect 192.168.1.100:4443 -showcerts
```

## Network Configuration

- **IP Address**: 192.168.1.100
- **Gateway**: 192.168.1.1
- **Subnet**: 255.255.255.0
- **Port**: 4443 (HTTPS standard)
- **Protocol**: TLS 1.2+ with ECDHE-ECDSA

Ensure your device is on the same network segment (192.168.1.x).

## Serial Output

Monitor the device with:
```bash
python -m serial.tools.miniterm COM<N> 115200
```

You should see messages like:
```
[device] Starting HTTPS Server on port 4443
[device] Available endpoints:
[device]   GET  /              -> HTML page
[device]   GET  /main.js       -> JavaScript
[device]   GET  /device-info   -> Device information (JSON)
[device]   GET/POST /echo      -> Echo server
[device] Setting up TLS credentials
```

## Project Structure

```
_12_https_server_basic/
├── src/
│   ├── main.c                          # HTTPS server with TLS implementation
│   ├── certs/
│   │   ├── generate_certificates.py    # Auto-generates certificates (idempotent)
│   │   ├── server_cert.der             # Server certificate (generated once)
│   │   ├── server_privkey.der          # Server private key (generated once)
│   │   ├── ca_cert.der                 # CA certificate (generated once)
│   │   └── dummy_psk.h                 # Pre-shared key for PSK support
│   └── static_web_resources/
│       ├── index.html                  # Web interface with security indicators
│       └── main.js                     # Client-side logic (compressed)
├── CMakeLists.txt                      # Build configuration with cert generation
├── Kconfig                             # Kernel options
├── prj.conf                            # Zephyr configuration for HTTPS
└── sections-rom.ld                     # Linker script for HTTP resources
```

## Comparison: HTTP vs HTTPS

| Aspect | HTTP (_11) | HTTPS (_12) |
|--------|-----------|-----------|
| Port | 8080 | 4443 |
| Encryption | None (plaintext) | TLS/SSL (encrypted) |
| Certificates | Not needed | Required (auto-generated) |
| Performance | Faster (no crypto overhead) | Slight overhead (encryption/decryption) |
| Security | ❌ Unencrypted traffic | ✅ All data encrypted |
| Use Case | Testing/Development | Production/Sensitive Data |

## Certificate Lifecycle

**First Build:**
```
1. CMake detects missing server_cert.der and server_privkey.der
2. Runs: python generate_certificates.py
3. Certificates created with 2020-2100 validity window
4. Files written to src/certs/
5. App compiles and links successfully
```

**Subsequent Builds:**
```
1. CMake checks if certificates exist
2. They already exist → script skips regeneration
3. Existing certificates are reused
4. App compiles with stable certificate state
```

**To Regenerate Certificates:**
```bash
# Delete the .der files
rm apps/networking/ETHERNET/_12_https_server_basic/src/certs/*.der

# Next build will regenerate them
west build -b stm32h573i_dk apps/networking/ETHERNET/_12_https_server_basic
```

## Learning Points

This example demonstrates:
- HTTPS/TLS protocol implementation in Zephyr
- Certificate generation and management
- Secure socket communication patterns
- Self-signed certificate usage for embedded systems
- TLS handshake and key exchange
- Encrypted client-server communication
- Real-time monitoring via secure web interface
- Static resource serving over HTTPS with compression

## Troubleshooting

**"Certificate verification failed"**
- Normal with self-signed certs. Use browser's "Proceed anyway" or `curl -k`

**"Handshake failed"**
- Check that certificates exist in `src/certs/`
- Verify port 4443 is not in use
- Check serial output for TLS setup errors

**"Connection refused"**
- Verify device IP is 192.168.1.100
- Ensure device is powered and Ethernet connected
- Check network connectivity with `ping 192.168.1.100`

**Certificates not regenerating**
- Intentional design - delete `.der` files to force regeneration
- This prevents breaking TLS with constantly changing certificates


