# Example 3: Ethernet with Static IP Configuration

## Description

This example builds on **Example 2** by adding IP address management.

**Key Features:**
- Detects link state changes using PHY callbacks (from Example 2)
- Automatically assigns a **static IP address** when cable is connected
- Automatically removes the IP address when cable is disconnected
- Displays interface information and IP assignment status

**IP Configuration:**
```
IP Address:  192.168.1.100
Netmask:     255.255.255.0
Gateway:     192.168.1.1
```

## What you will see

### Expected output:

```
*** Booting Zephyr OS build v4.3.0-2952-ge855856a5d63 ***
[00:00:00.061,000] <inf> eth_static: === STM32H573 Ethernet Static IP Test ===
[00:00:00.069,000] <inf> eth_static: Network interface found: 0x20001088
[00:00:00.077,000] <inf> eth_static: MAC Address: 02:80:E1:26:6D:D5
[00:00:00.085,000] <inf> eth_static: Interface type: ethernet
[00:00:00.092,000] <inf> eth_static: PHY device found: eth_stm32_hal
[00:00:00.101,000] <inf> eth_static: Registering PHY link state callback...
[00:00:00.109,000] <inf> eth_static: PHY callback registered successfully
[00:00:01.556,000] <inf> phy_mii: PHY (0) Link speed 100 Mb, full duplex
[00:00:01.564,000] <inf> eth_static: >>> LINK UP - Cable connected!
[00:00:01.571,000] <inf> eth_static: Static IP assigned: 192.168.1.100
[00:00:05.456,000] <inf> eth_static: >>> LINK DOWN - Cable disconnected!
[00:00:05.462,000] <inf> eth_static: Static IP removed
[00:00:08.789,000] <inf> eth_static: >>> LINK UP - Cable connected!
[00:00:08.796,000] <inf> eth_static: Static IP assigned: 192.168.1.100
```

## How to test it

### Requirements:
- Ethernet cable
- Serial monitor (115200 baud)
- Network access (recommended to test connectivity)

### Steps:
1. Build:
   ```bash
   west build -p always -b stm32h573i_dk apps/networking/ETHERNET/_03_static_ip
   ```

2. Flash:
   ```bash
   west flash
   ```

3. Open serial monitor at 115200 baud

4. Connect/disconnect the cable - you'll see:
   ```
   >>> LINK UP - Cable connected!
   Static IP assigned: 192.168.1.100
   ```

5. **Optional**: From another computer on the same network:
   ```bash
   ping 192.168.1.100
   ```

   The ping will work after you see "Static IP assigned"

## How it works

1. **PHY Link Detection**: Uses callbacks from Example 2 to detect physical cable connection
2. **IP Assignment**: When link is UP, assigns static IP `192.168.1.100`
3. **IP Removal**: When link is DOWN, removes the IP address


## References

- [Zephyr Network Interface API](https://docs.zephyrproject.org/latest/connectivity/networking/api/net_if.html)
- Example 2: Link detection with callbacks
