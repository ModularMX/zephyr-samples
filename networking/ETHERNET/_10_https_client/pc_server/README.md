# PC Server Files

This directory contains the HTTPS server and utilities for testing the HTTPS client example.

## Files

### 1. `https-server-ipv4.py`
**Python HTTPS Server Script**

A test HTTPS server that listens on all IPv4 interfaces (0.0.0.0:4443) for testing the Zephyr HTTPS client.

**Features:**
- Supports both GET and POST requests
- TLS 1.2 compatible with Zephyr/mbedTLS
- Detailed logging of incoming connections and requests
- Compatible cipher suites for embedded clients
- Automatic certificate loading from `https-server.pem`

**Usage:**
```bash
python https-server-ipv4.py
```

**Server Configuration:**
- Port: 4443
- Protocol: TLS 1.2
- Certificate Verification: None (for testing)
- Binding: 0.0.0.0 (all IPv4 interfaces)

### 2. `https-server.pem`
**SSL/TLS Server Certificate**

A self-signed certificate used by the HTTPS server for testing purposes. This is the same certificate from Zephyr's net-tools.

**Certificate Details:**
- Type: Self-signed X.509 certificate
- Format: PEM (combined certificate + private key)
- Common Name (CN): localhost
- Use: Development and testing only

**Note:** This certificate is **NOT suitable for production** use. For production environments, obtain a valid certificate from a trusted Certificate Authority (CA).

The corresponding CA certificate (`https-cert.der`) used by the Zephyr client is located in `../src/https-cert.der`.

### 3. `capture-live.bat`
**Wireshark Live Packet Capture Script (Windows)**

A batch script for capturing and displaying HTTPS traffic in real-time using Wireshark's tshark.

**Features:**
- Real-time packet capture on port 4443
- Filters TCP and TLS traffic
- Displays packets in a simple one-line format
- Useful for debugging TLS handshake and connection issues

**Usage:**
```bash
capture-live.bat
```

**Requirements:**
- Wireshark installed at: `C:\Program Files\Wireshark\`
- Administrator privileges (may be required for packet capture)
- Network interface configured as "Ethernet"

**Note:** Press Ctrl+C to stop the capture.

---

## Troubleshooting

### Server Won't Start
- Check if port 4443 is already in use: `netstat -an | findstr 4443`
- Verify `https-server.pem` exists in the same directory

### Client Can't Connect
- Verify PC firewall allows incoming connections on port 4443
- Check network connectivity: `ping 192.168.1.100`
- Ensure both devices are on the same subnet

### Certificate Errors
- Verify the CA certificate in `../src/https-cert.der` matches `https-server.pem`
- Check that `TLS_PEER_HOSTNAME` in `../src/ca_certificate.h` is set to "localhost"
