/**
 * STM32H573 - DHCP Client
 *
 * How DHCP works:
 * 1. Client sends DISCOVER (broadcasts "who has an IP for me?")
 * 2. Server sends OFFER (offers an available IP address)
 * 3. Client sends REQUEST (confirms it wants that IP)
 * 4. Server sends ACK (confirms, lease is now active)
 *
 * Result: The device gets an IP, netmask, gateway, and lease time automatically.
 */

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>

LOG_MODULE_REGISTER(eth_dhcp_client, LOG_LEVEL_INF);

// DHCP Option 42: NTP Server address (for time synchronization)
#define DHCP_OPTION_NTP (42)

// Buffer to store NTP server IP received from DHCP server
static uint8_t ntp_server[4];

// Event callback structures - used to listen for network events
static struct net_mgmt_event_callback mgmt_cb;    // Listens for IP address changes
static struct net_dhcpv4_option_callback dhcp_cb; // Listens for DHCP option updates

/**
 * Start DHCP client on a network interface
 *
 * This function is called by net_if_foreach() for each available
 * network interface in the system. We start the DHCP client
 * which will begin the DISCOVER/OFFER/REQUEST/ACK sequence.
 */
static void start_dhcpv4_client(struct net_if *iface, void *user_data)
{
    ARG_UNUSED(user_data);

    // Log which interface we're starting DHCP on
    LOG_INF("Starting DHCP on %s (index %d)",
            net_if_get_device(iface)->name,
            net_if_get_by_iface(iface));

    // Initiate the DHCP protocol sequence
    net_dhcpv4_start(iface);
}

/**
 * Handle NET_EVENT_IPV4_ADDR_ADD event
 *
 * Triggered when DHCP successfully assigns an IP address to the interface.
 * We extract and display the IP, subnet mask, gateway, and lease duration.
 */
static void handler(struct net_mgmt_event_callback *cb,
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

        // Skip addresses that are not from DHCP
        if (iface->config.ip.ipv4->unicast[i].ipv4.addr_type != NET_ADDR_DHCP)
        {
            continue;
        }

        // Log the assigned IP address
        LOG_INF("IP Address: %s",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
                              buf, sizeof(buf)));

        // Log the subnet mask
        LOG_INF("Netmask:    %s",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->unicast[i].netmask,
                              buf, sizeof(buf)));

        // Log the default gateway
        LOG_INF("Gateway:    %s",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->gw,
                              buf, sizeof(buf)));

        // Log lease duration: how long until we need to renew the IP
        LOG_INF("Lease time: %u seconds (%u days)",
                iface->config.dhcpv4.lease_time,
                iface->config.dhcpv4.lease_time / 86400);
    }
}

/**
 * Handle DHCP option responses
 *
 * Some DHCP servers send additional options like NTP servers (option 42).
 * This callback receives those optional parameters.
 */
static void option_handler(struct net_dhcpv4_option_callback *cb,
                           size_t length,
                           enum net_dhcpv4_msg_type msg_type,
                           struct net_if *iface)
{
    char buf[NET_IPV4_ADDR_LEN];

    // Convert the option data to human-readable IP address format
    LOG_INF("DHCP Option %d: %s",
            cb->option,
            net_addr_ntop(NET_AF_INET, cb->data, buf, sizeof(buf)));
}

int main(void)
{
    LOG_INF("STM32H573 DHCP Client");
    LOG_INF("Waiting for network interface...");

    // Register callback to be notified when an IPv4 address is assigned
    net_mgmt_init_event_callback(&mgmt_cb, handler, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&mgmt_cb);

    // Register callback to receive DHCP options (like NTP server)
    net_dhcpv4_init_option_callback(&dhcp_cb, option_handler,
                                    DHCP_OPTION_NTP, // Option type to listen for
                                    ntp_server,      // Buffer to store the option data
                                    sizeof(ntp_server));
    net_dhcpv4_add_option_callback(&dhcp_cb);

    // Start DHCP on all available network interfaces
    net_if_foreach(start_dhcpv4_client, NULL);

    // Process continues - kernel scheduler handles network events in background
    return 0;
}
