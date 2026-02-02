// HTTP Server Example for Zephyr
// "Hello World" - Simple HTTP server with device info and echo endpoints
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/net_config.h>

// HTTP server configuration
#define HTTP_SERVER_PORT 8080
#define MAX_HTTP_CLIENTS 4

// =============================================================================
// STATIC RESOURCES (Compressed with gzip)
// =============================================================================

// HTML content - compressed at build time
static const uint8_t index_html_gz[] = {
#include "index.html.gz.inc"
};

// JavaScript content - compressed at build time
static const uint8_t main_js_gz[] = {
#include "main.js.gz.inc"
};

// =============================================================================
// RESOURCE DEFINITIONS
// =============================================================================

// Index.html resource - served at "/" endpoint
// Client sends: GET /
// Server responds: HTML page (gzipped)
static struct http_resource_detail_static index_html_resource_detail = {
	.common = {
		.type = HTTP_RESOURCE_TYPE_STATIC,
		.bitmask_of_supported_http_methods = BIT(HTTP_GET),
		.content_encoding = "gzip",
		.content_type = "text/html",
	},
	.static_data = index_html_gz,
	.static_data_len = sizeof(index_html_gz),
};

// Main.js resource - served at "/main.js" endpoint
// Client sends: GET /main.js
// Server responds: JavaScript file (gzipped)
static struct http_resource_detail_static main_js_resource_detail = {
	.common = {
		.type = HTTP_RESOURCE_TYPE_STATIC,
		.bitmask_of_supported_http_methods = BIT(HTTP_GET),
		.content_encoding = "gzip",
		.content_type = "text/javascript",
	},
	.static_data = main_js_gz,
	.static_data_len = sizeof(main_js_gz),
};

// =============================================================================
// DYNAMIC RESOURCE HANDLERS
// =============================================================================

// Device info handler - returns device information as JSON
// Client sends: GET /device-info
// Server responds: JSON with board, architecture, uptime, status
static int device_info_handler(struct http_client_ctx *client, enum http_data_status status,
							   const struct http_request_ctx *request_ctx,
							   struct http_response_ctx *response_ctx, void *user_data)
{
	// Buffer for JSON response
	static char info_buf[256];
	int len;

	printk("[HTTP] Device info handler called, status: %d\n", status);

	// Wait for all request data to be received before responding
	if (status == HTTP_SERVER_DATA_FINAL)
	{
		// Build JSON response with device information
		len = snprintf(info_buf, sizeof(info_buf),
					   "{\"board\":\"STM32H573I-DK\","
					   "\"arch\":\"ARM Cortex-M33\","
					   "\"uptime\":%" PRId64 ","
					   "\"status\":\"Running\"}",
					   k_uptime_get());

		if (len < 0 || len >= (int)sizeof(info_buf))
		{
			printk("[ERR] Failed to format device info\n");
			return -ENOMEM;
		}

		// Set response body and mark as complete
		response_ctx->body = (uint8_t *)info_buf;
		response_ctx->body_len = len;
		response_ctx->final_chunk = true;
	}

	return 0;
}

static struct http_resource_detail_dynamic device_info_resource_detail = {
	.common = {
		.type = HTTP_RESOURCE_TYPE_DYNAMIC,
		.bitmask_of_supported_http_methods = BIT(HTTP_GET),
	},
	.cb = device_info_handler,
	.user_data = NULL,
};

// Uptime handler - returns device uptime in milliseconds
// Client sends: GET /uptime
// Server responds: Plain text number (milliseconds since boot)
static int uptime_handler(struct http_client_ctx *client, enum http_data_status status,
			  const struct http_request_ctx *request_ctx,
			  struct http_response_ctx *response_ctx, void *user_data)
{
	int ret;
	static uint8_t uptime_buf[sizeof(STRINGIFY(INT64_MAX))];

	printk("[HTTP] Uptime handler called, status: %d\n", status);

	// Wait for all request data to be received before responding
	// This is important for large requests that arrive in chunks
	if (status == HTTP_SERVER_DATA_FINAL) {
		// Format the uptime (milliseconds since boot) as a string
		ret = snprintf(uptime_buf, sizeof(uptime_buf), "%" PRId64, k_uptime_get());
		if (ret < 0) {
			printk("[ERR] Failed to format uptime, err %d\n", ret);
			return ret;
		}

		// Set response body and mark as complete
		response_ctx->body = uptime_buf;
		response_ctx->body_len = ret;
		response_ctx->final_chunk = true;
	}

	return 0;
}

static struct http_resource_detail_dynamic device_info_resource_detail = {
	.common = {
		.type = HTTP_RESOURCE_TYPE_DYNAMIC,
		.bitmask_of_supported_http_methods = BIT(HTTP_GET),
	},
	.cb = device_info_handler,
	.user_data = NULL,
};

// Echo handler - echoes back the received data
// Client sends: GET /echo or POST /echo with body
// Server responds: Same body data back to client
static int echo_handler(struct http_client_ctx *client, enum http_data_status status,
			const struct http_request_ctx *request_ctx,
			struct http_response_ctx *response_ctx, void *user_data)
{
	enum http_method method = client->method;

	// Handle aborted transactions (connection closed by client)
	if (status == HTTP_SERVER_DATA_ABORTED) {
		printk("[HTTP] Echo transaction aborted\n");
		return 0;
	}

	// Log received data
	if (request_ctx->data_len > 0) {
		printk("[HTTP] %s received %zd bytes\n", http_method_str(method), request_ctx->data_len);
	}

	// Echo data back to client
	response_ctx->body = request_ctx->data;
	response_ctx->body_len = request_ctx->data_len;
	// Mark as final chunk if all data received
	response_ctx->final_chunk = (status == HTTP_SERVER_DATA_FINAL);

	return 0;
}

static struct http_resource_detail_dynamic echo_resource_detail = {
	.common = {
		.type = HTTP_RESOURCE_TYPE_DYNAMIC,
		.bitmask_of_supported_http_methods = BIT(HTTP_GET) | BIT(HTTP_POST),
	},
	.cb = echo_handler,
	.user_data = NULL,
};

// =============================================================================
// HTTP SERVICE CONFIGURATION
// =============================================================================

// Define the HTTP service
// - Listens on all interfaces (NULL = bind to all)
// - Port: 8080
// - Max 4 simultaneous clients
// - Max 10 streams per client
static uint16_t http_service_port = HTTP_SERVER_PORT;
HTTP_SERVICE_DEFINE(http_service, NULL, &http_service_port,
		    MAX_HTTP_CLIENTS, 10, NULL, NULL, NULL);

// =============================================================================
// RESOURCE ROUTING
// =============================================================================

// Route "/" -> static HTML
HTTP_RESOURCE_DEFINE(index_resource, http_service, "/", &index_html_resource_detail);

// Route "/main.js" -> static JavaScript
HTTP_RESOURCE_DEFINE(main_js_resource, http_service, "/main.js", &main_js_resource_detail);

// Route "/device-info" -> dynamic endpoint (calls device_info_handler)
HTTP_RESOURCE_DEFINE(device_info_resource, http_service, "/device-info", &device_info_resource_detail);

// Route "/echo" -> dynamic endpoint (echoes back data)
HTTP_RESOURCE_DEFINE(echo_resource, http_service, "/echo", &echo_resource_detail);

// =============================================================================
// MAIN APPLICATION
// =============================================================================

int main(void)
{
	printk("[HTTP] Starting HTTP Server on port %d\n", HTTP_SERVER_PORT);
	printk("[HTTP] Available endpoints:\n");
	printk("[HTTP]   GET  /              -> HTML page\n");
	printk("[HTTP]   GET  /main.js       -> JavaScript\n");
	printk("[HTTP]   GET  /device-info   -> Device information (JSON)\n");
	printk("[HTTP]   GET/POST /echo      -> Echo server\n");
	
	// Start the HTTP server (blocking call)
	http_server_start();
	
	return 0;
}
