/**
 * Example 8: UDP Receiver - Based on Nordic Pattern
 *
 * Simple UDP receiver that binds to a port and receives datagrams
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/socket.h>
#include <zephyr/posix/arpa/inet.h>
#include <zephyr/posix/unistd.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

LOG_MODULE_REGISTER(eth_udp_receiver, LOG_LEVEL_INF);

// ============================================================================
// UDP SERVER CONFIGURATION - Customize these values for your setup
// ============================================================================
#define LOCAL_PORT 4242
#define BOARD_IP_ADDR htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL)
#define BOARD_IP_MASK 24
#define BUFFER_SIZE 256

// Event callback structure
static struct net_mgmt_event_callback mgmt_cb;

// Socket descriptor
static int sock = -1;
static uint8_t recv_buf[BUFFER_SIZE];

/**
 * Assign static IP address to interface
 *
 * Uses BOARD_IP_ADDR and BOARD_IP_MASK defines to configure the board's IP
 *
 * To change the IP address, modify BOARD_IP_ADDR define at the top of the file
 */
static void assign_static_ip(struct net_if *iface)
{
    struct in_addr addr;
    char buf[NET_IPV4_ADDR_LEN];

    addr.s_addr = BOARD_IP_ADDR;

    struct net_if_addr *ifaddr = net_if_ipv4_addr_add(iface, &addr, NET_ADDR_MANUAL, BOARD_IP_MASK);

    if (!ifaddr)
    {
        LOG_ERR("Failed to add IPv4 address");
        return;
    }

    LOG_INF("Board IP assigned: %s/%d",
            net_addr_ntop(NET_AF_INET, &addr, buf, sizeof(buf)),
            BOARD_IP_MASK);
}

/**
 * Handle network events
 */
static void ip_event_handler(struct net_mgmt_event_callback *cb,
                             uint64_t mgmt_event,
                             struct net_if *iface)
{
    if (mgmt_event != NET_EVENT_IPV4_ADDR_ADD)
    {
        return;
    }

    LOG_INF("IPv4 address event received");
}

/**
 * UDP Receive
 *
 * Binds to a port and listens for incoming UDP packets
 */
static void udp_receive(void)
{
    struct sockaddr_in addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char client_ip[NET_IPV4_ADDR_LEN];
    int received;

    LOG_INF("Creating UDP socket...");

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0)
    {
        LOG_ERR("Failed to create socket: errno %d, %s", errno, strerror(errno));
        return;
    }
    LOG_INF("Socket created successfully");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LOCAL_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        LOG_ERR("Failed to bind socket: errno %d, %s", errno, strerror(errno));
        close(sock);
        return;
    }
    LOG_INF("Socket bound to 0.0.0.0:%d", LOCAL_PORT);
    LOG_INF("Listening for UDP packets...");

    while (1)
    {
        received = recvfrom(sock, recv_buf, sizeof(recv_buf) - 1, 0,
                            (struct sockaddr *)&client_addr, &client_addr_len);

        if (received < 0)
        {
            LOG_ERR("Socket error: %d", errno);
            break;
        }

        if (received == 0)
        {
            LOG_WRN("Empty datagram");
            continue;
        }

        recv_buf[received] = '\0';

        // Convert sender IP to string
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

        LOG_INF("Received %d bytes from %s:%d",
                received, client_ip, ntohs(client_addr.sin_port));
        LOG_INF("  Data: %s", (char *)recv_buf);
    }

    close(sock);
}

int main(void)
{
    struct net_if *iface;

    LOG_INF("UDP Receiver with Static IP");

    // Get the default network interface
    iface = net_if_get_default();
    if (!iface)
    {
        LOG_ERR("ERROR: No network interface found!");
        return 1;
    }

    LOG_INF("Network interface found");

    // Register callback to be notified when an IPv4 address is assigned
    net_mgmt_init_event_callback(&mgmt_cb, ip_event_handler, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&mgmt_cb);

    // Assign static IP address
    assign_static_ip(iface);

    // Wait a moment for the IP to be fully assigned
    k_sleep(K_SECONDS(2));

    // Start receiving UDP packets
    udp_receive();

    return 0;
}
