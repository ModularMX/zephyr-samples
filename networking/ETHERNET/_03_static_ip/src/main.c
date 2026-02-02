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
#include <zephyr/net/net_mgmt.h>


#define MAC_ADDR_LEN 6

LOG_MODULE_REGISTER(eth_static, LOG_LEVEL_INF);

// Event callback structure - used to listen for IP address changes
static struct net_mgmt_event_callback mgmt_cb;

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
        printk("Failed to add IPv4 address\n");
        return;
    }

    LOG_INF("Static IP assigned: 192.168.1.100/24");
    printk("Static IP assigned: 192.168.1.100/24\n");
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
    printk("Static IP removed\n");
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
        printk(">>> LINK UP - Cable connected!\n");
        assign_static_ip(iface);
    }
    else
    {
        printk(">>> LINK DOWN - Cable disconnected!\n");
        remove_static_ip(iface);
    }
}

/**
 * Handle NET_EVENT_IPV4_ADDR_ADD event
 *
 * Notifies when a static IP address is successfully assigned to the interface.
 */
static void ip_event_handler(struct net_mgmt_event_callback *cb,
                             uint64_t mgmt_event,
                             struct net_if *iface)
{
    int i = 0;

    // Only handle IP address add events
    if (mgmt_event != NET_EVENT_IPV4_ADDR_ADD)
    {
        return;
    }

    // Iterate through all IPv4 addresses on this interface
    for (i = 0; i < NET_IF_MAX_IPV4_ADDR; i++)
    {
        char buf[NET_IPV4_ADDR_LEN];

        // Skip addresses that are not manually assigned
        if (iface->config.ip.ipv4->unicast[i].ipv4.addr_type != NET_ADDR_MANUAL)
        {
            continue;
        }

        // Log the assigned static IP address
        printk("IP Address: %s\n",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
                              buf, sizeof(buf)));

        // Log the subnet mask
        printk("Netmask:    %s\n",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->unicast[i].netmask,
                              buf, sizeof(buf)));

        // Log the default gateway
        printk("Gateway:    %s\n",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->gw,
                              buf, sizeof(buf)));
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

    LOG_INF("Network interface found: %p", iface);
    printk("Network interface found: %p\n", iface);

    // Register callback to be notified when an IPv4 address is assigned
    net_mgmt_init_event_callback(&mgmt_cb, ip_event_handler, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&mgmt_cb);

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
        LOG_WRN("Could not read MAC address");
        printk("Could not read MAC address\n");
    }

    // Get interface type
    LOG_INF("Interface type: %s", iface->if_dev->dev->name);
    printk("Interface type: %s\n", iface->if_dev->dev->name);

    // Get the PHY device from the interface
    const struct device *phy_dev = net_eth_get_phy(iface);
    if (!phy_dev || !device_is_ready(phy_dev))
    {
        printk("ERROR: PHY device not ready!\n");
        return 1;
    }

    LOG_INF("PHY device found: %s", phy_dev->name);
    printk("PHY device found: %s\n", phy_dev->name);

    // Register PHY link state callback
    printk("Registering PHY link state callback...\n");
    phy_link_callback_set(phy_dev, phy_link_state_changed, (void *)iface);
    printk("PHY callback registered successfully\n");

    printk("Waiting for link state changes...\n");
    printk("Connect/disconnect cable to assign/remove IP\n");

    // Wait for callbacks
    while (1)
    {
        k_sleep(K_FOREVER);
    }

    return 0;
}
