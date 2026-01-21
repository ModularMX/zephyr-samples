/**
 * Example 5: Generic TCP Client with Static IP
 *
 * Demonstrates a reusable TCP client implementation compatible with any Zephyr board
 *
 * TCP Socket Lifecycle:
 * 1. Assign static IP address to network interface
 * 2. Create socket (AF_INET, SOCK_STREAM for TCP)
 * 3. Connect to server (configurable IP:port)
 * 4. Send data to server
 * 5. Receive response from server
 * 6. Close connection gracefully
 *
 * Board Support: Any Zephyr board with Ethernet and networking support
 * Tested on: STM32H573I-DK, Nordic nRF52, QEMU, and others
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

LOG_MODULE_REGISTER(eth_tcp_client, LOG_LEVEL_INF);

// ============================================================================
// TCP SERVER CONFIGURATION - Customize these values for your setup
// ============================================================================
#define SERVER_PORT 4242                   // TCP server port
#define SERVER_ADDR "192.168.1.1"          // TCP server IP address
#define SEND_DATA "Hello from Zephyr!"     // Message to send

// ============================================================================
// BOARD IP CONFIGURATION - Customize these values for your network
// ============================================================================
// IP Address (replace numbers to change: 192.168.1.100 → 10.0.0.50 etc)
#define BOARD_IP_ADDR htonl((192UL << 24) | (168UL << 16) | (1UL << 8) | 100UL)
#define BOARD_IP_MASK 24                   // Netmask: /24 (255.255.255.0)

// ============================================================================
// INTERNAL CONSTANTS - Do not modify
// ============================================================================
#define RECV_BUF_SIZE 1024

// Event callback structure - used to listen for IP address changes
static struct net_mgmt_event_callback mgmt_cb;

// Socket descriptor (global so we can use it in callbacks)
static int tcp_socket = -1;

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

    // Use the BOARD_IP_ADDR define which already contains the full network byte order conversion
    addr.s_addr = BOARD_IP_ADDR;

    // Add address with prefix length from BOARD_IP_MASK define (/24 = 255.255.255.0)
    struct net_if_addr *ifaddr = net_if_ipv4_addr_add(iface, &addr, NET_ADDR_MANUAL, BOARD_IP_MASK);

    if (!ifaddr)
    {
        LOG_ERR("Failed to add IPv4 address");
        return;
    }

    LOG_INF("Static IP assigned: 192.168.1.100/%d", BOARD_IP_MASK);
}

/**
 * Handle NET_EVENT_IPV4_ADDR_ADD event
 *
 * Called when the device gets an IP address (static in this case).
 * Displays the assigned IP and initiates TCP connection.
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
        LOG_INF("IP Address: %s",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
                              buf, sizeof(buf)));

        // Log the subnet mask
        LOG_INF("Netmask:    %s",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->unicast[i].netmask,
                              buf, sizeof(buf)));

        // Log the gateway
        LOG_INF("Gateway:    %s",
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->gw,
                              buf, sizeof(buf)));

        LOG_INF("Ready to connect to TCP server!");
    }
}

/**
 * Create and connect a TCP socket to the server
 *
 * This function demonstrates the complete TCP client sequence:
 * 1. Create socket
 * 2. Prepare server address structure
 * 3. Connect to server
 * 4. Send data
 * 5. Receive response
 * 6. Close connection
 */
static void tcp_connect_and_send(void)
{
    struct sockaddr_in server_addr;
    char recv_buffer[RECV_BUF_SIZE];
    int ret;

    LOG_INF("Creating TCP socket...");

    // Create a TCP socket (AF_INET = IPv4, SOCK_STREAM = TCP)
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0)
    {
        LOG_ERR("Failed to create socket: %d", -errno);
        return;
    }
    LOG_INF("Socket created successfully");

    // Prepare the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT); // Port number in network byte order

    // Convert server IP address from string to binary format
    ret = inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
    if (ret <= 0)
    {
        LOG_ERR("Invalid server address: %s", SERVER_ADDR);
        close(tcp_socket);
        return;
    }
    LOG_INF("Server address: %s:%d", SERVER_ADDR, SERVER_PORT);

    // Attempt to connect to the server
    LOG_INF("Connecting to server...");
    ret = connect(tcp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        int err_code = errno;
        LOG_ERR("Connection failed: errno %d", err_code);

        // Print descriptive error messages
        switch (err_code)
        {
        case 113: // EHOSTUNREACH
            LOG_ERR("  Host unreachable!");
            LOG_ERR("  - Check if Docker is running");
            LOG_ERR("  - Check Ethernet cable connection");
            break;
        case 111: // ECONNREFUSED
            LOG_ERR("  Connection refused!");
            LOG_ERR("  - TCP server not running on Docker");
            break;
        case 110: // ETIMEDOUT
            LOG_ERR("  Connection timeout!");
            LOG_ERR("  - Server not responding");
            break;
        default:
            LOG_ERR("  Unknown error (errno: %d)", err_code);
            break;
        }

        close(tcp_socket);
        tcp_socket = -1;
        return;
    }
    LOG_INF("Connected to server!");

    // Send data to the server
    LOG_INF("Sending: '%s'", SEND_DATA);
    ret = send(tcp_socket, SEND_DATA, sizeof(SEND_DATA) - 1, 0);
    if (ret < 0)
    {
        LOG_ERR("Send failed: %d", -errno);
        close(tcp_socket);
        tcp_socket = -1;
        return;
    }
    LOG_INF("Data sent (%d bytes)", ret);

    // Receive response from server
    LOG_INF("Waiting for response...");
    ret = recv(tcp_socket, recv_buffer, RECV_BUF_SIZE - 1, 0);
    if (ret < 0)
    {
        LOG_ERR("Receive failed: %d", -errno);
        close(tcp_socket);
        tcp_socket = -1;
        return;
    }

    // Null-terminate the received data
    recv_buffer[ret] = '\0';

    // Display the received data
    LOG_INF("Received (%d bytes): '%s'", ret, recv_buffer);

    // Close the socket
    close(tcp_socket);
    tcp_socket = -1;
    LOG_INF("Connection closed");
}

int main(void)
{
    struct net_if *iface;

    LOG_INF("TCP Client with Static IP");

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
    k_sleep(K_SECONDS(1));

    // Attempt to connect to TCP server
    tcp_connect_and_send();

    // Keep the system running
    LOG_INF("Waiting...");
    while (1)
    {
        k_sleep(K_FOREVER);
    }

    return 0;
}
