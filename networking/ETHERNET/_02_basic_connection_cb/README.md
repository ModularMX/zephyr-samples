# Example 2: Basic Ethernet Connection with Callbacks

## Description

This example is similar to **Example 1** but uses **event callbacks** instead of **polling** to detect link state changes.

**Key Difference:**
- Example 1: Polls every 2 seconds (checks, checks, checks...)
- Example 2: Uses callbacks (waits for events to happen automatically)

**Features:**
- Initializes the Ethernet driver
- Gets and displays the MAC address
- Detects link state changes using **callbacks** (event-driven)
- More efficient - no busy-waiting
- Production-ready pattern

## What you will see?

### Expected output:

```
*** Booting Zephyr OS build v4.3.0-2952-ge855856a5d63 ***
[00:00:00.061,000] <inf> eth_basic: === STM32H573 Ethernet Basic Connection Test ===
[00:00:00.069,000] <inf> eth_basic: Waiting for PHY to negotiate link...
[00:00:01.556,000] <inf> phy_mii: PHY (0) Link speed 100 Mb, full duplex
[00:00:02.089,000] <inf> eth_basic: Network interface found: 0x20001088
[00:00:02.097,000] <inf> eth_basic: MAC Address: 02:80:E1:26:6D:D5
[00:00:02.106,000] <inf> eth_basic: Interface type: ethernet
[00:00:02.112,000] <inf> eth_basic: Waiting for link state changes (using callbacks, not polling)...
[00:00:02.121,000] <inf> eth_basic: >>> LINK UP - Cable connected!
[00:00:05.456,000] <inf> eth_basic: >>> LINK DOWN - Cable disconnected!
[00:00:08.789,000] <inf> eth_basic: >>> LINK UP - Cable connected!
```

Notice: **Changes appear instantly** when you connect/disconnect the cable (not waiting for the next 2-second poll).

## How to test it

### Requirements:
- Ethernet cable
- Serial monitor (115200 baud)

### Steps:
1. Build:
   ```bash
   west build -p always -b stm32h573i_dk apps/networking/ETHERNET/_02_basic_connection_cb
   ```

2. Flash:
   ```bash
   west flash
   ```

3. Open the serial monitor at 115200 baud

4. Test by **quickly** disconnecting/connecting the cable - you'll see **instant** responses:
   ```
   [00:00:xx.xxx] <inf> eth_basic: >>> LINK UP - Cable connected!
   [00:00:xx.xxx] <inf> eth_basic: >>> LINK DOWN - Cable disconnected!
   ```

## References

- [Zephyr Network Management API](https://docs.zephyrproject.org/latest/connectivity/networking/api/net_mgmt.html)
- [Zephyr Network Events](https://docs.zephyrproject.org/latest/connectivity/networking/api/net_mgmt.html#_CPPv428NET_EVENT_L2_CONNECTED)
- Example 1 (polling) - for comparison

## Important Note: STM32H573 Driver Limitations

**As of January 16, 2026**, the STM32H573 Ethernet driver (`eth_stm32_hal_v2.c`) **does NOT expose PHY link state change events** to the Zephyr management event system.

### What this means:

**Other Ethernet drivers** (ESP32, NXP S32, Renesas RA, SAM, etc.) implement the full event chain:
```
PHY Link Change → phy_link_callback_set() → net_eth_carrier_on/off() → NET_EVENT_ETHERNET_CARRIER_ON/OFF
```

**STM32H573 driver (v2)** only supports:
```
PHY Link Change → phy_link_callback_set() [ONLY THIS]
```

### Workaround:

This example uses **direct PHY callbacks** (`phy_link_callback_set()`) instead of relying on management events. This is actually more efficient and lower-level, but it's a limitation of the current driver implementation.

### Impact on other examples:

- **IP-layer events** (DHCP, IPv4 address assignment, etc.) should still work normally
- **Link-layer events** must be handled through PHY callbacks (as shown in this example)
- Future examples will combine both approaches: PHY callbacks for link detection + management events for IP events

