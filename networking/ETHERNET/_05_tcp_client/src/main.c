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
        printk("Failed to add IPv4 address\n");
        return;
    }

    printk("Static IP assigned: 192.168.1.100/%d\n", BOARD_IP_MASK);
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
        printk("IP Address: %s\n",
            net_addr_ntop(NET_AF_INET,
                      &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
                      buf, sizeof(buf)));

        // Log the subnet mask
        printk("Netmask:    %s\n",
            net_addr_ntop(NET_AF_INET,
                      &iface->config.ip.ipv4->unicast[i].netmask,
                      buf, sizeof(buf)));

        // Log the gateway
        printk("Gateway:    %s\n",
            net_addr_ntop(NET_AF_INET,
                      &iface->config.ip.ipv4->gw,
                      buf, sizeof(buf)));

        printk("Ready to connect to TCP server!\n");
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

    printk("Creating TCP socket...\n");

    // Create a TCP socket (AF_INET = IPv4, SOCK_STREAM = TCP)
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0)
    {
        printk("Failed to create socket: %d\n", -errno);
        return;
    }
    printk("Socket created successfully\n");

    // Prepare the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT); // Port number in network byte order

    // Convert server IP address from string to binary format
    ret = inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
    if (ret <= 0)
    {
        printk("Invalid server address: %s\n", SERVER_ADDR);
        close(tcp_socket);
        return;
    }
    printk("Server address: %s:%d\n", SERVER_ADDR, SERVER_PORT);

    // Attempt to connect to the server
    printk("Connecting to server...\n");
    ret = connect(tcp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        int err_code = errno;
        printk("Connection failed: errno %d\n", err_code);

        // Print descriptive error messages
        switch (err_code)
        {
        case 113: // EHOSTUNREACH
            printk("  Host unreachable!\n");
            printk("  - Check if Docker is running\n");
            printk("  - Check Ethernet cable connection\n");
            break;
        case 111: // ECONNREFUSED
            printk("  Connection refused!\n");
            printk("  - TCP server not running on Docker\n");
            break;
        case 110: // ETIMEDOUT
            printk("  Connection timeout!\n");
            printk("  - Server not responding\n");
            break;
        default:
            printk("  Unknown error (errno: %d)\n", err_code);
            break;
        }

        close(tcp_socket);
        tcp_socket = -1;
        return;
    }
    printk("Connected to server!\n");

    // Send data to the server
    printk("Sending: '%s'\n", SEND_DATA);
    ret = send(tcp_socket, SEND_DATA, sizeof(SEND_DATA) - 1, 0);
    if (ret < 0)
    {
        printk("Send failed: %d\n", -errno);
        close(tcp_socket);
        tcp_socket = -1;
        return;
    }
    printk("Data sent (%d bytes)\n", ret);

    // Receive response from server
    printk("Waiting for response...\n");
    ret = recv(tcp_socket, recv_buffer, RECV_BUF_SIZE - 1, 0);
    if (ret < 0)
    {
        printk("Receive failed: %d\n", -errno);
        close(tcp_socket);
        tcp_socket = -1;
        return;
    }

    // Null-terminate the received data
    recv_buffer[ret] = '\0';

    // Display the received data
    printk("Received (%d bytes): '%s'\n", ret, recv_buffer);

    // Close the socket
    close(tcp_socket);
    tcp_socket = -1;
    printk("Connection closed\n");
}

int main(void)
{
    struct net_if *iface;

    printk("TCP Client with Static IP\n");

    // Get the default network interface
    iface = net_if_get_default();
    if (!iface)
    {
        printk("ERROR: No network interface found!\n");
        return 1;
    }

    printk("Network interface found\n");

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
    printk("Waiting...\n");
    while (1)
    {
        k_sleep(K_FOREVER);
    }

    return 0;
}
