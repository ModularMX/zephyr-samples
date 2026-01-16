#include <zephyr/kernel.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/logging/log.h>

/**
 * @brief Basic Ethernet connection test application for STM32H573
 */
#define MAC_ADDR_LEN 6

LOG_MODULE_REGISTER(eth_basic, LOG_LEVEL_INF);

int main(void)
{
    struct net_if *iface;
    bool last_state = false; // Track previous link state
    bool current_state;

    LOG_INF("=== STM32H573 Ethernet Basic Connection Test ===");

    // Give PHY time to negotiate link (typically ~1 second)
    LOG_INF("Waiting for PHY to negotiate link...");
    k_sleep(K_SECONDS(2));

    // Get the default network interface
    iface = net_if_get_default();
    if (!iface)
    {
        LOG_ERR("ERROR: No network interface found!");
        return 1;
    }

    LOG_INF("Network interface found: %p", iface);

    // Get and display MAC address
    const struct net_linkaddr *linkaddr = net_if_get_link_addr(iface);
    if (linkaddr && MAC_ADDR_LEN == linkaddr->len)
    {
        LOG_INF("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
                linkaddr->addr[0], linkaddr->addr[1], linkaddr->addr[2],
                linkaddr->addr[3], linkaddr->addr[4], linkaddr->addr[5]);
    }
    else
    {
        LOG_WRN("Could not read MAC address");
    }

    // Get interface type
    LOG_INF("Interface type: %s", iface->if_dev->dev->name);

    LOG_INF("Starting link state polling (checking every 2 seconds)...\n");

    // Polling loop - checks link state periodically
    while (1)
    {
        // Get current link state
        current_state = net_if_is_up(iface);

        // Detect state change and log it
        if (current_state != last_state)
        {
            if (current_state)
            {
                LOG_INF(">>> LINK UP - Cable connected!");
            }
            else
            {
                LOG_INF(">>> LINK DOWN - Cable disconnected!");
            }
            last_state = current_state;
        }

        // Sleep before next check
        k_sleep(K_SECONDS(2));
    }

    return 0;
}

