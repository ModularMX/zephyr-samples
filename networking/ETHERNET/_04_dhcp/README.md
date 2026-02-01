# Example 4: Ethernet with DHCP Client

## Description

This example demonstrates how to obtain an IP address dynamically using DHCP.

**Key Features:**
- Detects link state changes using PHY callbacks
- Requests an IP address from a DHCP server when the cable is connected
- Releases the IP address when the cable is disconnected
- Prints assigned IP address and network info

## What you will see

### Expected output:

```
*** Booting Zephyr OS build ... ***
Network interface found: ...
MAC Address: ...
Interface type: ethernet
PHY device found: ...
Registering PHY link state callback...
PHY callback registered successfully
[00:00:01.556,000] <inf> phy_mii: PHY (0) Link speed 100 Mb, full duplex
>>> LINK UP - Cable connected!
DHCP request sent
DHCP assigned IP: 192.168.1.101
Netmask: 255.255.255.0
Gateway: 192.168.1.1
>>> LINK DOWN - Cable disconnected!
DHCP lease released
```

## Build & Flash
```bash
west build -b <BOARD> apps/networking/ETHERNET/_04_dhcp
west flash
```

## Monitor Output
```bash
python -m serial.tools.miniterm COMx 115200 --eol LF
```
Replace <BOARD> and COMx with your hardware.
