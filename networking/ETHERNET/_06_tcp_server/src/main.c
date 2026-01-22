/**
 * Example 6: Generic TCP Server with Static IP (Simple Sequenced)
 *
 * Demonstrates a simple TCP server implementation - accepts one client at a time
 *
 * TCP Server Lifecycle:
 * 1. Assign static IP address to network interface
 * 2. Create TCP socket
 * 3. Bind socket to IP:port
 * 4. Listen for incoming connections
 * 5. Accept client connection
 * 6. Receive data from client
 * 7. Echo data back to client
 * 8. Close client connection
 * 9. Go back to step 5 (repeat)
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

LOG_MODULE_REGISTER(eth_tcp_server, LOG_LEVEL_INF);

// ============================================================================
// TCP SERVER CONFIGURATION - Customize these values for your setup
// ============================================================================
#define SERVER_PORT 5555                   // TCP server listening port
#define BACKLOG 1                          // Maximum clients in queue

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
#define LISTEN_QUEUE_SIZE 1

// Event callback structure - used to listen for IP address changes
static struct net_mgmt_event_callback mgmt_cb;

// Socket descriptors
static int server_socket = -1;

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
 * Echo handler - receives data from client and sends it back
 *
 * This is a simple sequential server that handles one client at a time
 */
static void handle_client(int client_socket, struct sockaddr_in *client_addr)
{
    char recv_buffer[RECV_BUF_SIZE];
    int received;
    int sent;
    char client_ip[INET_ADDRSTRLEN];

    // Convert client IP to string for logging
    inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, INET_ADDRSTRLEN);

    LOG_INF("Client connected: %s:%d", client_ip, ntohs(client_addr->sin_port));

    // Echo loop - receive data and send it back
    while (1)
    {
        // Receive data from client
        received = recv(client_socket, recv_buffer, RECV_BUF_SIZE - 1, 0);

        if (received < 0)
        {
            LOG_ERR("Receive failed: %d", -errno);
            break;
        }

        if (received == 0)
        {
            // Client closed connection
            LOG_INF("Client disconnected");
            break;
        }

        // Null-terminate the received data for logging
        recv_buffer[received] = '\0';

        LOG_INF("Received (%d bytes): '%s'", received, recv_buffer);

        // Send data back to client (echo)
        sent = send(client_socket, recv_buffer, received, 0);
        if (sent < 0)
        {
            LOG_ERR("Send failed: %d", -errno);
            break;
        }

        LOG_INF("Echoed (%d bytes) back to client", sent);
    }

    // Close client connection
    close(client_socket);
    LOG_INF("Client socket closed\n");
}

/**
 * Create and start TCP server
 *
 * This function sets up the server socket, binds to port, and accepts clients
 */
static void start_tcp_server(void)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int client_socket;
    int ret;
    int optval;

    LOG_INF("Creating TCP server socket...");

    // Create a TCP socket (AF_INET = IPv4, SOCK_STREAM = TCP)
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        LOG_ERR("Failed to create socket: %d", -errno);
        return;
    }
    LOG_INF("Server socket created successfully");

    // Allow socket address reuse (useful for restarts)
    optval = 1;
    ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (ret < 0)
    {
        LOG_WRN("Failed to set SO_REUSEADDR: %d", -errno);
    }

    // Prepare the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on all interfaces
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind socket to port
    LOG_INF("Binding to port %d...", SERVER_PORT);
    ret = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        LOG_ERR("Failed to bind socket: %d", -errno);
        close(server_socket);
        return;
    }
    LOG_INF("Socket bound successfully");

    // Listen for incoming connections
    LOG_INF("Listening for incoming connections...");
    ret = listen(server_socket, LISTEN_QUEUE_SIZE);
    if (ret < 0)
    {
        LOG_ERR("Failed to listen on socket: %d", -errno);
        close(server_socket);
        return;
    }
    LOG_INF("Server listening on 0.0.0.0:%d\n", SERVER_PORT);

    // Accept and handle clients (infinite loop)
    while (1)
    {
        // Accept incoming client connection
        client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_socket < 0)
        {
            LOG_ERR("Failed to accept connection: %d", -errno);
            continue;
        }

        // Handle the connected client
        handle_client(client_socket, &client_addr);
    }

    // Note: This code will never be reached (infinite loop above)
    close(server_socket);
}

int main(void)
{
    struct net_if *iface;

    LOG_INF("TCP Server with Static IP (Generic - compatible with all boards)");

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

    // Start TCP server
    start_tcp_server();

    return 0;
}
