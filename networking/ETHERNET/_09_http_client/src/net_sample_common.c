#include <zephyr/net/conn_mgr_connectivity.h>
#include <zephyr/net/conn_mgr_monitor.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/kernel.h>

#define MAC_ADDR_LEN 6

static struct net_mgmt_event_callback l4_cb;
static K_SEM_DEFINE(network_connected, 0, 1);

static void assign_static_ip(struct net_if *iface)
{
    struct in_addr addr;
    addr.s_addr = htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL);
    struct net_if_addr *ifaddr = net_if_ipv4_addr_add(iface, &addr, NET_ADDR_MANUAL, 24);
    if (!ifaddr)
    {
        printk("Failed to add IPv4 address\n");
        return;
    }
    printk("Static IP assigned: 192.168.1.100/24\n");
}

static void l4_event_handler(struct net_mgmt_event_callback *cb, uint64_t event, struct net_if *iface)
{
    switch (event) {
    case NET_EVENT_L4_CONNECTED:
        printk("Network connectivity established and IP address assigned\n");

        char buf[NET_IPV4_ADDR_LEN];

        // Log the assigned static IP address
        printk("IP Address: %s\n",
               net_addr_ntop(NET_AF_INET,
                             &iface->config.ip.ipv4->unicast[0].ipv4.address.in_addr,
                             buf, sizeof(buf)));

        // Log the subnet mask
        printk("Netmask:    %s\n",
               net_addr_ntop(NET_AF_INET,
                             &iface->config.ip.ipv4->unicast[0].netmask,
                             buf, sizeof(buf)));

        // Log the gateway
        printk("Gateway:    %s\n",
               net_addr_ntop(NET_AF_INET,
                             &iface->config.ip.ipv4->gw,
                             buf, sizeof(buf)));

        k_sem_give(&network_connected);
        break;
    case NET_EVENT_L4_DISCONNECTED:
        break;
    default:
        break;
    }
}

void wait_for_network(void)
{
    // Get the default network interface
    struct net_if *iface = net_if_get_default();
    if (!iface)
    {
        printk("ERROR: No network interface found!\n");
        return;
    }

    // Print MAC address
    const struct net_linkaddr *linkaddr = net_if_get_link_addr(iface);
    if (linkaddr && MAC_ADDR_LEN == linkaddr->len)
    {
        printk("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
               linkaddr->addr[0], linkaddr->addr[1], linkaddr->addr[2],
               linkaddr->addr[3], linkaddr->addr[4], linkaddr->addr[5]);
    }

    // Register L4 connectivity event callback
    net_mgmt_init_event_callback(&l4_cb, l4_event_handler, NET_EVENT_L4_CONNECTED | NET_EVENT_L4_DISCONNECTED);
    net_mgmt_add_event_callback(&l4_cb);
    conn_mgr_mon_resend_status();
    printk("Waiting for network...\n");

    // Assign static IP for the sake of the example
    assign_static_ip(iface);
    k_sem_take(&network_connected, K_FOREVER);
}
