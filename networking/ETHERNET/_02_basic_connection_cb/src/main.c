/**
 * @brief Ethernet connection test with link state callbacks
 * Uses PHY-level callbacks to detect cable connect/disconnect
 */

#include <zephyr/kernel.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/phy.h>
#include <zephyr/logging/log.h>

#define MAC_ADDR_LEN 6

LOG_MODULE_REGISTER(eth_basic, LOG_LEVEL_INF);

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
        LOG_INF(">>> LINK UP - Cable connected!");
    }
    else
    {
        LOG_INF(">>> LINK DOWN - Cable disconnected!");
    }
}

int main(void)
{
    struct net_if *iface;

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

    // Get the PHY device from the interface
    const struct device *phy_dev = net_eth_get_phy(iface);
    if (!phy_dev || !device_is_ready(phy_dev))
    {
        LOG_ERR("ERROR: PHY device not ready!");
        return 1;
    }

    LOG_INF("PHY device found: %s", phy_dev->name);

    // Register PHY link state callback
    LOG_INF("Registering PHY link state callback...");
    phy_link_callback_set(phy_dev, phy_link_state_changed, (void *)iface);
    LOG_INF("PHY callback registered successfully");

    LOG_INF("Waiting for link state changes (callback-driven)...");
    LOG_INF("Connect/disconnect cable to see events\n");

    // Just wait - callbacks will handle everything!
    while (1)
    {
        k_sleep(K_FOREVER);
    }

    return 0;
}
