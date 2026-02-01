/*
 * HTTPS Client Example for Zephyr
 * Based on Zephyr's http_client sample with TLS support
 * Simplified for Ethernet with static IP configuration
 */

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/posix/sys/socket.h>
#include <zephyr/net/http/client.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/phy.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/icmp.h>

#include <zephyr/net/net_mgmt.h>
#include <zephyr/posix/arpa/inet.h> // for inet_pton
#include <zephyr/posix/unistd.h>    // for close
#include <string.h>
#include <stdio.h>
#include "net_sample_common.h"
#include "ca_certificate.h"

#define SERVER_IP "192.168.1.1"     // Change to your PC/server IP
#define SERVER_PORT 4443            // HTTPS port
#define RECV_BUF_SIZE 512

static uint8_t recv_buf[RECV_BUF_SIZE];

static int response_cb(struct http_response *rsp, enum http_final_call final_data, void *user_data)
{
    switch (final_data)
    {
    case HTTP_DATA_MORE:
        printk("[HTTPS] Partial data received (%zd bytes)\n", rsp->data_len);
        break;
    case HTTP_DATA_FINAL:
        printk("[HTTPS] All data received (%zd bytes)\n", rsp->data_len);
        break;
    default:
        break;
    }
    printk("[HTTPS] Status: %s\n", rsp->http_status);

    // Print only the body fragment (may be called multiple times)
    if (rsp->body_frag_len > 0 && rsp->body_frag_start)
    {
        printk("[HTTPS] Body fragment: %.*s\n", (int)rsp->body_frag_len, rsp->body_frag_start);
    }
    return 0;
}

/**
 * @brief Create and configure a TLS socket for HTTPS communication
 *
 * @param server Server IP address (string)
 * @param port Server port
 * @param sock Pointer to socket descriptor (output)
 * @param addr Pointer to sockaddr_in structure (output)
 *
 * @return 0 on success, -1 on failure
 */
static int setup_tls_socket(const char *server, int port, int *sock, struct sockaddr_in *addr)
{
    int ret = 0;
    sec_tag_t sec_tag_list[] = {
        CA_CERTIFICATE_TAG,
    };

    // Initialize address structure
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);

    // Convert IP address string to binary form
    if (inet_pton(AF_INET, server, &addr->sin_addr) <= 0)
    {
        printk("[ERR] Invalid IP address: %s\n", server);
        return -1;
    }

    // Create TLS socket (TLS 1.2)
    *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
    if (*sock < 0)
    {
        printk("[ERR] Failed to create TLS socket (%d)\n", errno);
        return -1;
    }

    // Set security tag for CA certificate
    ret = setsockopt(*sock, SOL_TLS, TLS_SEC_TAG_LIST, sec_tag_list, sizeof(sec_tag_list));
    if (ret < 0)
    {
        printk("[ERR] Failed to set TLS security tag (%d)\n", errno);
        close(*sock);
        return -1;
    }

    // Set TLS hostname for server name indication
    // NOTE: Using sizeof instead of strlen like Zephyr does
    ret = setsockopt(*sock, SOL_TLS, TLS_HOSTNAME, TLS_PEER_HOSTNAME, sizeof(TLS_PEER_HOSTNAME));
    if (ret < 0)
    {
        printk("[ERR] Failed to set TLS hostname (%d)\n", errno);
        close(*sock);
        return -1;
    }

    return ret;
}

/**
 * @brief Setup TLS socket and connect to server
 *
 * This function combines TLS socket creation and connection to the server.
 *
 * @param server Server IP address (string)
 * @param port Server port
 * @param sock Pointer to socket descriptor (output)
 * @param addr Pointer to sockaddr_in structure (output)
 *
 * @return 0 on success, -1 on failure
 */
static int connect_tls_socket(const char *server, int port, int *sock, struct sockaddr_in *addr)
{
    int ret;

    // First setup the TLS socket
    ret = setup_tls_socket(server, port, sock, addr);
    if (ret < 0 || *sock < 0)
    {
        return -1;
    }

    // Then connect to server
    ret = connect(*sock, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        printk("[ERR] Failed to connect (%d)\n", errno);
        close(*sock);
        *sock = -1;
        return -1;
    }

    return 0;
}

/**
 * @brief Test basic TCP connectivity (without TLS)
 * This helps diagnose network issues before TLS
 */
static void test_basic_connectivity(const char *server_ip, int port)
{
    int sock;
    struct sockaddr_in addr;
    int ret;

    printk("\n--- Testing Basic TCP Connectivity ---\n");
    printk("Target: %s:%d\n", server_ip, port);

    // Create plain TCP socket (no TLS)
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        printk("[ERR] Failed to create TCP socket (%d)\n", errno);
        return;
    }

    // Setup address
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) <= 0) {
        printk("[ERR] Invalid IP address: %s\n", server_ip);
        close(sock);
        return;
    }

    // Try to connect
    printk("[TEST] Attempting TCP connection...\n");
    ret = connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    
    if (ret < 0) {
        printk("[ERR] TCP connection failed (%d) - errno: %d\n", ret, errno);
        printk("      This indicates network routing problem\n");
        close(sock);
        return;
    }

    printk("[OK] TCP connection successful!\n");
    printk("     Network routing is working\n");
    close(sock);
}

int main(void)
{
    // Wait for network connectivity
    wait_for_network();

    struct sockaddr_in server_addr;
    int sock = -1;
    int ret;
    struct http_request req;
    int32_t timeout = 3000; // ms

    printk("\n--- Zephyr HTTPS Client Example ---\n");

    // Test basic connectivity first
    test_basic_connectivity(SERVER_IP, SERVER_PORT);

    // Register CA certificate
    ret = tls_credential_add(CA_CERTIFICATE_TAG,
                           TLS_CREDENTIAL_CA_CERTIFICATE,
                           ca_certificate,
                           sizeof(ca_certificate));
    if (ret < 0)
    {
        printk("[ERR] Failed to register CA certificate: %d\n", ret);
        return 1;
    }

    printk("[HTTPS] CA certificate registered\n");
    printk("Connecting to %s:%d\n", SERVER_IP, SERVER_PORT);

    // ===== HTTPS GET REQUEST =====
    // Setup TLS socket and connect
    ret = connect_tls_socket(SERVER_IP, SERVER_PORT, &sock, &server_addr);
    if (ret < 0)
    {
        return 0;
    }

    // Prepare GET request
    memset(&req, 0, sizeof(req));
    req.method = HTTP_GET;
    req.url = "/";
    req.host = SERVER_IP;
    req.protocol = "HTTP/1.1";
    req.response = response_cb;
    req.recv_buf = recv_buf;
    req.recv_buf_len = sizeof(recv_buf);

    printk("[HTTPS] Sending GET request...\n");
    ret = http_client_req(sock, &req, timeout, "GET");
    if (ret < 0)
    {
        printk("[ERR] HTTPS client error %d\n", ret);
    }

    // Close GET connection
    close(sock);

    // Small delay between requests
    k_sleep(K_MSEC(500));

    // ===== HTTPS POST REQUEST =====
    // Setup new TLS socket and connect
    ret = connect_tls_socket(SERVER_IP, SERVER_PORT, &sock, &server_addr);
    if (ret < 0)
    {
        return 0;
    }

    // Prepare POST request
    const char *post_payload = "{\"message\":\"Hello from Zephyr via HTTPS\",\"board\":\"STM32H573I-DK\",\"secure\":true}";
    const char *content_type = "application/json";

    memset(&req, 0, sizeof(req));
    req.method = HTTP_POST;
    req.url = "/";
    req.host = SERVER_IP;
    req.protocol = "HTTP/1.1";
    req.response = response_cb;
    req.recv_buf = recv_buf;
    req.recv_buf_len = sizeof(recv_buf);
    req.content_type_value = content_type;
    req.payload = post_payload;
    req.payload_len = strlen(post_payload);

    printk("[HTTPS] Sending POST request...\n");
    ret = http_client_req(sock, &req, timeout, "POST");
    if (ret < 0)
    {
        printk("[ERR] HTTPS client error %d\n", ret);
    }

    close(sock);
    printk("[HTTPS] Done.\n");
}
