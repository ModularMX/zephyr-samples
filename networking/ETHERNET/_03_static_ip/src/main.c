/**
 * @brief Ethernet with Static IP Configuration
 * Demonstrates assigning a static IP address and monitoring connection
 */

#include <zephyr/kernel.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/phy.h>
#include <zephyr/logging/log.h>

#define MAC_ADDR_LEN 6

LOG_MODULE_REGISTER(eth_static, LOG_LEVEL_INF);

/**
 * @brief Assign static IP address to interface
 * IP: 192.168.1.100/24
 */
static void assign_static_ip(struct net_if *iface)
{
    struct in_addr addr;
    
    /* Build address 192.168.1.100 */
    addr.s_addr = htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL);
    
    /* Add address with /24 prefix (255.255.255.0) */
    struct net_if_addr *ifaddr = net_if_ipv4_addr_add(iface, &addr, NET_ADDR_MANUAL, 24);
    
    if (!ifaddr) {
        LOG_ERR("Failed to add IPv4 address");
        return;
    }

    LOG_INF("Static IP assigned: 192.168.1.100/24");
}

/**
 * @brief Remove static IP address from interface
 */
static void remove_static_ip(struct net_if *iface)
{
    struct in_addr addr;
    
    /* Build address 192.168.1.100 */
    addr.s_addr = htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL);

    net_if_ipv4_addr_rm(iface, &addr);
    LOG_INF("Static IP removed");
}

/**
 * @brief PHY link state change callback
 */
static void phy_link_state_changed(const struct device *phy_dev,
                                   struct phy_link_state *state,
                                   void *user_data)
{
    struct net_if *iface = (struct net_if *)user_data;

    ARG_UNUSED(phy_dev);

    if (state->is_up)
    {
        LOG_INF(">>> LINK UP - Cable connected!");
        assign_static_ip(iface);
    }
    else
    {
        LOG_INF(">>> LINK DOWN - Cable disconnected!");
        remove_static_ip(iface);
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

    LOG_INF("Waiting for link state changes...");
    LOG_INF("Connect/disconnect cable to assign/remove IP\n");

    // Wait for callbacks
    while (1)
    {
        k_sleep(K_FOREVER);
    }

    return 0;
}
