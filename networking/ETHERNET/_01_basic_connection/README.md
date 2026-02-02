# Example 1: Basic Ethernet Connection Validation

## Description

This example validates that the Ethernet hardware works correctly on the STM32H573I-DK.

**Features:**
- Initializes the Ethernet driver
- Gets and displays the MAC address (physical address of the card)
- Detects the link state (UP/DOWN) continuously
- Notifies you of changes when you disconnect/connect the cable

##  What you will see?

### Expected output:

```
*** Booting Zephyr OS build v4.3.0-2952-ge855856a5d63 *** 
=== STM32H573 Ethernet Basic Connection Test ===
Waiting for PHY to negotiate link...
[00:00:01.556,000] <inf> phy_mii: PHY (0) Link speed 100 Mb, full duplex
Network interface found: 0x20001088
MAC Address: 02:80:E1:26:6D:D5
Interface type: ethernet
Starting link state polling (checking every 2 seconds)...
>>> LINK UP - Cable connected!
```

## How to test it

### Requirements:
- Ethernet cable connected to a router/switch with DHCP
- Serial monitor (115200 baud)

### Steps:
1. Build:
   ```bash
   west build -p always -b stm32h573i_dk apps/networking/ETHERNET/_01_basic_connection
   ```

2. Flash:
   ```bash
   west flash
   ```

3. Open the serial monitor at 115200 baud

4. Test by disconnecting/connecting the cable - you'll see:
   ```
   >>> LINK UP - Cable connected!
   >>> LINK DOWN - Cable disconnected!
   ```

## Ethernet Concepts for Beginners

### What is Ethernet?
Ethernet is a networking standard that enables communication between devices through cables. It's what devices use to connect to the internet or other devices.

### Main layers:

```
Application (HTTP, TCP, UDP)
    ↓
Network     (IP, DHCP)
    ↓
Link        (Ethernet, MAC address) ← This example validates here
    ↓
Physical    (Cable, PHY negotiation) ← And here
```

### Key concepts:

#### **MAC Address (Physical address)**
- Unique 48-bit identifier (6 bytes)
- Example: `02:80:E1:26:6D:D5`
- Works at the link layer (before IP)
- Each Ethernet card has a unique MAC address

#### **PHY (Physical Layer)**
- Chip that handles the physical signal of the cable
- Negotiates speed: 10 Mbps, 100 Mbps, or 1 Gbps
- Detects if the cable is connected
- Validates that communication is stable

#### **Link State (UP/DOWN)**
- **UP**: Cable connected + PHY negotiated successfully
  - Meaning: I can communicate
  - Does NOT mean: I have an IP or internet
  
- **DOWN**: Cable disconnected or no negotiation
  - Meaning: No access to the physical medium
  - Nothing works until it's UP

### Initialization flow:

```
Boot
  ↓
Zephyr loads network stack
  ↓
Ethernet driver initializes (gets MAC)
  ↓
PHY starts negotiation (takes ~1 second)
  ↓
Your code runs (sees LINK state)
  ↓
If UP: you can communicate on the network
If DOWN: cable not connected
```

## Code Structure

**main.c:**
- `net_if_get_default()`: Gets the Ethernet interface
- `net_if_get_link_addr()`: Reads the MAC address
- `net_if_is_up()`: Checks if the link is UP
- **Polling loop**: Every 2 seconds checks for state changes

**prj.conf:**
- `CONFIG_NETWORKING=y`: Enables network stack
- `CONFIG_NET_L2_ETHERNET=y`: Enables Ethernet layer
- `CONFIG_ETH_DRIVER=y`: Generic driver
- `CONFIG_ETH_STM32_HAL=y`: STM32-specific driver
- Buffers and statistics for packet handling

**stm32h573i_dk.overlay:**
- RMII pin configuration (already in .dtsi, no changes needed)
- PHY is at address 0x00


## References

- [Zephyr Networking Documentation](https://docs.zephyrproject.org/latest/connectivity/networking/index.html)
- [Zephyr Ethernet Driver](https://docs.zephyrproject.org/latest/connectivity/networking/api/ethernet.html)
- IEEE 802.3 (Ethernet standard)

