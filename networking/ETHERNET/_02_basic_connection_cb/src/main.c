/**
 * @brief Ethernet connection test with link state callbacks
 * Uses PHY-level callbacks to detect cable connect/disconnect
 */

#include <zephyr/kernel.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/phy.h>


#define MAC_ADDR_LEN 6



/**
 * @brief PHY link state change callback
 * Called by PHY driver when link state changes (cable connect/disconnect)
 */
static void phy_link_state_changed(const struct device *phy_dev,
                                   struct phy_link_state *state,
                                   void *user_data)
{
    ARG_UNUSED(phy_dev);
    ARG_UNUSED(user_data);

    if (state->is_up)
    {
		printk(">>> LINK UP - Cable connected!\n");
    }
    else
    {
		printk(">>> LINK DOWN - Cable disconnected!\n");
    }
}

int main(void)
{
    struct net_if *iface;

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

    // Get the PHY device from the interface
    const struct device *phy_dev = net_eth_get_phy(iface);
    if (!phy_dev || !device_is_ready(phy_dev))
    {
		printk("ERROR: PHY device not ready!\n");
        return 1;
    }

	printk("PHY device found: %s\n", phy_dev->name);

    // Register PHY link state callback
	printk("Registering PHY link state callback...\n");
    phy_link_callback_set(phy_dev, phy_link_state_changed, (void *)iface);
	printk("PHY callback registered successfully\n");

	printk("Waiting for link state changes (callback-driven)...\n");
	printk("Connect/disconnect cable to see events\n");

    // Just wait - callbacks will handle everything!
    while (1)
    {
        k_sleep(K_FOREVER);
    }

    return 0;
}
