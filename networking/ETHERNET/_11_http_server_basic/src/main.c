// HTTP Server Example for Zephyr
// Simple HTTP server with static and dynamic resources
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/net_config.h>
#include <zephyr/logging/log.h>

// HTTP server configuration
#define HTTP_SERVER_PORT 8080
#define MAX_HTTP_CLIENTS 4

LOG_MODULE_REGISTER(http_server_sample, LOG_LEVEL_DBG);

// =============================================================================
// STATIC RESOURCES (Compressed with gzip)
// =============================================================================

// HTML content - compressed at build time
static uint8_t index_html_gz[] = {
#include "index.html.gz.inc"
};

// JavaScript content - compressed at build time
static uint8_t main_js_gz[] = {
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

// Uptime handler - returns device uptime in milliseconds
// Client sends: GET /uptime
// Server responds: Plain text number (milliseconds since boot)
static int uptime_handler(struct http_client_ctx *client, enum http_data_status status,
			  const struct http_request_ctx *request_ctx,
			  struct http_response_ctx *response_ctx, void *user_data)
{
	int ret;
	static uint8_t uptime_buf[sizeof(STRINGIFY(INT64_MAX))];

	LOG_DBG("Uptime handler called, status: %d", status);

	// Wait for all request data to be received before responding
	// This is important for large requests that arrive in chunks
	if (status == HTTP_SERVER_DATA_FINAL) {
		// Format the uptime (milliseconds since boot) as a string
		ret = snprintf(uptime_buf, sizeof(uptime_buf), "%" PRId64, k_uptime_get());
		if (ret < 0) {
			LOG_ERR("Failed to format uptime, err %d", ret);
			return ret;
		}

		// Set response body and mark as complete
		response_ctx->body = uptime_buf;
		response_ctx->body_len = ret;
		response_ctx->final_chunk = true;
	}

	return 0;
}

static struct http_resource_detail_dynamic uptime_resource_detail = {
	.common = {
		.type = HTTP_RESOURCE_TYPE_DYNAMIC,
		.bitmask_of_supported_http_methods = BIT(HTTP_GET),
	},
	.cb = uptime_handler,
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
		LOG_DBG("Echo transaction aborted");
		return 0;
	}

	// Log received data
	if (request_ctx->data_len > 0) {
		LOG_DBG("%s received %zd bytes", http_method_str(method), request_ctx->data_len);
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

// Route "/uptime" -> dynamic endpoint (calls uptime_handler)
HTTP_RESOURCE_DEFINE(uptime_resource, http_service, "/uptime", &uptime_resource_detail);

// Route "/echo" -> dynamic endpoint (echoes back data)
HTTP_RESOURCE_DEFINE(echo_resource, http_service, "/echo", &echo_resource_detail);

// =============================================================================
// MAIN APPLICATION
// =============================================================================

int main(void)
{
	LOG_INF("Starting HTTP Server on port %d", HTTP_SERVER_PORT);
	LOG_INF("Available endpoints:");
	LOG_INF("  GET  /              -> HTML page");
	LOG_INF("  GET  /main.js       -> JavaScript");
	LOG_INF("  GET  /uptime        -> Device uptime");
	LOG_INF("  GET/POST /echo      -> Echo server");
	
	// Start the HTTP server (blocking call)
	http_server_start();
	
	return 0;
}
