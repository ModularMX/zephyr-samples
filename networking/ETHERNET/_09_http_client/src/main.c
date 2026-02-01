/*
 * Simple HTTP Client Example for Zephyr
 * Based on Zephyr's http_client sample, simplified and using printk
 */

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/http/client.h>
#include <zephyr/net/net_ip.h>
#include <string.h>
#include <stdio.h>

#define SERVER_IP "192.168.1.100" // Change to your PC/server IP
#define SERVER_PORT 8000
#define RECV_BUF_SIZE 512

static uint8_t recv_buf[RECV_BUF_SIZE];

static int response_cb(struct http_response *rsp, enum http_final_call final_data, void *user_data)
{
    if (final_data == HTTP_DATA_MORE)
    {
        printk("[HTTP] Partial data received (%zd bytes)\n", rsp->data_len);
    }
    else if (final_data == HTTP_DATA_FINAL)
    {
        printk("[HTTP] All data received (%zd bytes)\n", rsp->data_len);
    }
    printk("[HTTP] Status: %s\n", rsp->http_status);
    if (rsp->data_len > 0)
    {
        printk("[HTTP] Body: %.*s\n", (int)rsp->data_len, rsp->body);
    }
    return 0;
}

void main(void)
{
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
        return;
    }

    ret = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        printk("[ERR] Failed to connect (%d)\n", errno);
        close(sock);
        return;
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
