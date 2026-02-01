
/*
 * Simple HTTP Client Example for Zephyr
 * Based on Zephyr's http_client sample, simplified and using printk
 * Assigns static IP and waits for network connectivity before HTTP request
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

#include <zephyr/net/net_mgmt.h>
#include <zephyr/posix/arpa/inet.h> // for inet_pton
#include <zephyr/posix/unistd.h>    // for close
#include <string.h>
#include <stdio.h>
#include "net_sample_common.h"

#define SERVER_IP "192.168.1.1" // Change to your PC/server IP
#define SERVER_PORT 8000
#define RECV_BUF_SIZE 512

static uint8_t recv_buf[RECV_BUF_SIZE];

static int response_cb(struct http_response *rsp, enum http_final_call final_data, void *user_data)
{
    switch (final_data)
    {
    case HTTP_DATA_MORE:
        printk("[HTTP] Partial data received (%zd bytes)\n", rsp->data_len);
        break;
    case HTTP_DATA_FINAL:
        printk("[HTTP] All data received (%zd bytes)\n", rsp->data_len);
        break;
    default:
        break;
    }
    printk("[HTTP] Status: %s\n", rsp->http_status);

    // Print only the body fragment (may be called multiple times)
    if (rsp->body_frag_len > 0 && rsp->body_frag_start)
    {
        printk("[HTTP] Body fragment: %.*s\n", (int)rsp->body_frag_len, rsp->body_frag_start);
    }
    return 0;
}

int main(void)
{

    // Wait for network connectivity (semaphore)
    wait_for_network();

    struct sockaddr_in server_addr;
    int sock = -1;
    int ret;
    struct http_request req;
    int32_t timeout = 3000; // ms

    printk("\n--- Zephyr HTTP Client Example ---\n");
    printk("Connecting to %s:%d\n", SERVER_IP, SERVER_PORT);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        printk("[ERR] Failed to create socket (%d)\n", errno);
        return 0;
    }

    ret = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        printk("[ERR] Failed to connect (%d)\n", errno);
        close(sock);
        return 0;
    }

    memset(&req, 0, sizeof(req));
    req.method = HTTP_GET;
    req.url = "/";
    req.host = SERVER_IP;
    req.protocol = "HTTP/1.1";
    req.response = response_cb;
    req.recv_buf = recv_buf;
    req.recv_buf_len = sizeof(recv_buf);

    printk("[HTTP] Sending GET request...\n");
    ret = http_client_req(sock, &req, timeout, "GET");
    if (ret < 0)
    {
        printk("[ERR] HTTP client error %d\n", ret);
    }

    close(sock);
    printk("[HTTP] Done.\n");
}
