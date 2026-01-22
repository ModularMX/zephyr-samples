/**
 * UDP Sender with Static IP
 *
 * Demonstrates a simple UDP sender implementation compatible with any Zephyr board
 *
 * UDP Socket Lifecycle:
 * 1. Assign static IP address to network interface
 * 2. Create socket (AF_INET, SOCK_DGRAM for UDP)
 * 3. Send data to server (connectionless - no connect needed)
 * 4. Repeat sending periodically
 * 5. Close socket when done
 *
 * UDP Advantages:
 * - Connectionless (no handshake)
 * - Lower latency
 * - Lower overhead
 * - No guaranteed delivery (fire and forget)
 *
 */

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/socket.h>
#include <zephyr/posix/sys/socket.h>
#include <zephyr/posix/arpa/inet.h>
#include <zephyr/posix/unistd.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

LOG_MODULE_REGISTER(eth_udp_sender, LOG_LEVEL_INF);

// ============================================================================
// UDP SERVER CONFIGURATION - Customize these values for your setup
// ============================================================================
#define SERVER_PORT 4242                   // UDP server port
#define SERVER_ADDR "192.168.1.1"          // PC host IP
#define SEND_DATA "Hello from Zephyr UDP!" // Message to send
#define SEND_INTERVAL_MS 2000              // Send every 2 seconds

// ============================================================================
// BOARD IP CONFIGURATION - Customize these values for your network
// ============================================================================
// IP Address (replace numbers to change: 192.168.1.100 → 10.0.0.50 etc)
#define BOARD_IP_ADDR htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL)
#define BOARD_IP_MASK 24 // Netmask: /24 (255.255.255.0)

// ============================================================================
// INTERNAL CONSTANTS - Do not modify
// ============================================================================

// Event callback structure - used to listen for IP address changes
static struct net_mgmt_event_callback mgmt_cb;

// Socket descriptor (global so we can use it in callbacks)
static int udp_socket = -1;

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

    // Use the BOARD_IP_ADDR define which already contains the full network byte order conversion
    addr.s_addr = BOARD_IP_ADDR;

    // Add address with prefix length from BOARD_IP_MASK define (/24 = 255.255.255.0)
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
 * Handle network events (IP address added, removed, etc.)
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
 * UDP Send and Receive
 *
 * Sends UDP packets periodically to the server
 * Based on Zephyr's official UDP example which uses connect() before send()
 */
static void udp_send_packets(void)
{
    struct sockaddr_in server_addr;
    int ret;
    static uint32_t packet_count = 0;
    char buffer[256];

    LOG_INF("Creating UDP socket...");

    // Create a UDP socket with IPPROTO_UDP
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_socket < 0)
    {
        LOG_ERR("Failed to create socket: errno %d, %s", errno, strerror(errno));
        return;
    }
    LOG_INF("Socket created successfully");

    // Prepare the server address structure - MUST use inet_pton, not inet_addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    ret = inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
    if (ret <= 0)
    {
        LOG_ERR("Invalid server address: %s (inet_pton returned %d)", SERVER_ADDR, ret);
        close(udp_socket);
        return;
    }

    LOG_INF("Server address: %s:%d", SERVER_ADDR, SERVER_PORT);

    // Connect UDP socket - Zephyr REQUIRES this for UDP
    LOG_INF("Connecting UDP socket...");
    ret = connect(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        LOG_ERR("Connect failed: errno %d, %s", errno, strerror(errno));
        close(udp_socket);
        udp_socket = -1;
        return;
    }
    LOG_INF("UDP socket connected!");

    // Send packets periodically
    LOG_INF("Starting UDP send loop (every %dms)...", SEND_INTERVAL_MS);

    for (int i = 0; i < 10; i++)
    {
        // Create message with packet counter
        snprintf(buffer, sizeof(buffer), "%s [packet #%u]", SEND_DATA, packet_count++);

        // Send using send() after connect()
        ret = send(udp_socket, buffer, strlen(buffer), 0);

        if (ret < 0)
        {
            LOG_ERR("Send failed: errno %d", errno);
            break;
        }

        LOG_INF("UDP packet #%u sent (%d bytes)", packet_count - 1, ret);

        // Wait before sending next packet
        k_msleep(SEND_INTERVAL_MS);
    }

    // Close the socket
    close(udp_socket);
    udp_socket = -1;
    LOG_INF("UDP socket closed");
}

int main(void)
{
    struct net_if *iface;

    LOG_INF("UDP Sender with Static IP");

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

    // Start sending UDP packets
    udp_send_packets();

    return 0;
}
