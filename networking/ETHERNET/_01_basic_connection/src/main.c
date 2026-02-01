/**
 * @brief Basic Ethernet connection test application for STM32H573
 */

#include <zephyr/kernel.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ethernet.h>


#define MAC_ADDR_LEN 6



int main(void)
{
    struct net_if *iface;
    bool last_state = false; // Track previous link state
    bool current_state;

	printk("=== STM32H573 Ethernet Basic Connection Test ===\n");

    // Give PHY time to negotiate link (typically ~1 second)
	printk("Waiting for PHY to negotiate link...\n");
    k_sleep(K_SECONDS(2));

    // Get the default network interface
    iface = net_if_get_default();
    if (!iface)
    {
		printk("ERROR: No network interface found!\n");
        return 1;
    }

	printk("Network interface found: %p\n", iface);

    // Get and display MAC address
    const struct net_linkaddr *linkaddr = net_if_get_link_addr(iface);
    if (linkaddr && MAC_ADDR_LEN == linkaddr->len)
    {
    		printk("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    			linkaddr->addr[0], linkaddr->addr[1], linkaddr->addr[2],
    			linkaddr->addr[3], linkaddr->addr[4], linkaddr->addr[5]);
    }
    else
    {
		printk("Could not read MAC address\n");
    }

    // Get interface type
	printk("Interface type: %s\n", iface->if_dev->dev->name);

	printk("Starting link state polling (checking every 2 seconds)...\n");

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
				printk(">>> LINK UP - Cable connected!\n");
            }
            else
            {
				printk(">>> LINK DOWN - Cable disconnected!\n");
            }
            last_state = current_state;
        }

        // Sleep before next check
        k_sleep(K_SECONDS(2));
    }

    return 0;
}

