# Example 9: Simple HTTP Client

This example performs a basic HTTP GET request to a server (e.g., your PC running a Python HTTP server).

## How to use

1. Start the Python HTTP server on your PC:
   ```bash
   python -m http.server 8000
   ```
   (By default, serves files from the current directory on port 8000)

2. Edit `main.c` and set `SERVER_IP` to your PC's IP address.

3. Build and flash:
   ```bash
   west build -b <BOARD> apps/networking/ETHERNET/_09_http_client
   west flash
   ```

4. Monitor serial output:
   ```bash
   python -m serial.tools.miniterm COMx 115200 --eol LF
   ```
   (Replace `<BOARD>` and `COMx` as needed)

## What it does
- Connects to the server via TCP
- Sends a GET request to `/`
- Prints the HTTP response using printk
