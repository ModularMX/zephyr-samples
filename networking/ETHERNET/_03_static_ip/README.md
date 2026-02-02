# Example 3: Ethernet with Static IP Configuration

## Description

This example builds on **Example 2** by adding IP address management.

**Key Features:**
- Detects link state changes using PHY callbacks (from Example 2)
- Automatically assigns a **static IP address** when cable is connected
- Automatically removes the IP address when cable is disconnected
- Monitors IP address events with callbacks
- Displays interface information and IP assignment status

**IP Configuration:**
```
IP Address:  192.168.1.100
Netmask:     255.255.255.0
```

## What you will see

### Expected output:

```
*** Booting Zephyr OS build v4.3.0-2952-ge855856a5d63 ***
Network interface found: 0x20001088
MAC Address: 02:80:E1:26:6D:D5
Interface type: ethernet
PHY device found: eth_stm32_hal
Registering PHY link state callback...
PHY callback registered successfully
[00:00:01.556,000] <inf> phy_mii: PHY (0) Link speed 100 Mb, full duplex
>>> LINK UP - Cable connected!
Static IP assigned: 192.168.1.100
IP Address: 192.168.1.100
Netmask:    255.255.255.0
>>> LINK DOWN - Cable disconnected!
Static IP removed
>>> LINK UP - Cable connected!
Static IP assigned: 192.168.1.100
IP Address: 192.168.1.100
Netmask:    255.255.255.0
```

## How to test it

### Requirements:
- Ethernet cable
- Serial monitor (115200 baud)
- Network interface (router or switch)

### Steps:
1. Build and flash the example
2. Open serial monitor
3. Connect/disconnect Ethernet cable to see IP assignment/removal
4. Observe IP event callbacks in the logs

## How it works

1. **PHY Link Detection**: Uses callbacks from Example 2 to detect physical cable connection
2. **IP Assignment**: When link is UP, assigns static IP `192.168.1.100/24`
3. **Event Monitoring**: Registers `NET_EVENT_IPV4_ADDR_ADD` callback to display assigned IP details
4. **IP Removal**: When link is DOWN, removes the IP address

## Key Code Concepts

- `net_if_ipv4_addr_add()`: Assigns static IP to interface
- `net_if_ipv4_addr_rm()`: Removes IP from interface
- `net_mgmt_init_event_callback()`: Registers for network events
- `NET_EVENT_IPV4_ADDR_ADD`: Event triggered when IP is assigned
- PHY callbacks: Hardware-level link state detection

## References

- [Zephyr Network Interface API](https://docs.zephyrproject.org/latest/connectivity/networking/api/net_if.html)
- [Zephyr Network Management Events](https://docs.zephyrproject.org/latest/connectivity/networking/api/net_mgmt.html)
- Example 2: Link detection with callbacks
