# Example 9: Simple HTTP Client

This example performs a basic HTTP GET request to a server running on your PC. It demonstrates static IP assignment, network connectivity waiting, and HTTP client functionality.

## Prerequisites

- STM32H573I-DK board connected via Ethernet
- Python 3.x on your PC

## How to use

### Step 1: Start the HTTP Server on your PC

First, navigate to the `pc_server/` directory and start the Python HTTP server:

```bash
cd apps/networking/ETHERNET/_09_http_client/pc_server
python3 simple_http_server.py
```

You should see:
```
Serving HTTP on 0.0.0.0:8000 (Ctrl+C to stop)
```

### Step 2: Configure the Client IP

Find your PC's Ethernet IPv4 address:

**Windows (PowerShell):**
```powershell
ipconfig
```

**Linux/Mac:**
```bash
ifconfig
# or
ip addr
```

Edit `src/main.c` and update `SERVER_IP` with your PC's IP address:
```c
#define SERVER_IP "192.168.1.1"  // Your PC's IP address
```

### Step 3: Build and Flash

```bash
west build -b stm32h573i_dk apps/networking/ETHERNET/_09_http_client
west flash
```

### Step 4: Monitor Serial Output

```bash
python -m serial.tools.miniterm COMx 115200 --eol LF
```

(Replace `COMx` with your serial port, e.g., `COM3`)

## Expected Output

```
*** Booting Zephyr OS build v4.3.0-2952-ge855856a5d63 ***
MAC Address: 02:80:E1:26:6D:D5
Waiting for network...
Static IP assigned: 192.168.1.100/24
[00:00:01.551,000] <inf> phy_mii: PHY (0) Link speed 100 Mb, full duplex
Network connectivity established and IP address assigned
IP Address: 192.168.1.100
Netmask:    255.255.255.0
Gateway:    0.0.0.0

--- Zephyr HTTP Client Example ---
Connecting to 192.168.1.1:8000
[HTTP] Sending GET request...
[HTTP] All data received (410 bytes)
[HTTP] Status: OK
[HTTP] Body fragment: <!DOCTYPE HTML>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Directory listing for /</title>
</head>
<body>
<h1>Directory listing for /</h1>
<hr>
<ul>
<li><a href="simple_http_server.py">simple_http_server.py</a></li>
</ul>
<hr>
</body>
</html>

[HTTP] Done.
```

## What it does

- Waits for network connectivity using a semaphore
- Assigns a static IPv4 address (`192.168.1.100/24`)
- Connects to the HTTP server via TCP
- Sends a GET request to `/`
- Receives and prints the HTTP response body using `printk`
- Demonstrates proper socket cleanup

## Key Features

- **Static IP Assignment**: Client uses fixed IP `192.168.1.100`
- **Network Wait**: Waits for Ethernet link and IP assignment before making the request
- **HTTP GET Request**: Sends HTTP GET to the root path `/`
- **Response Callback**: Prints received data fragments and HTTP status
- **POSIX API**: Uses standard `socket()`, `connect()`, and `close()` functions
