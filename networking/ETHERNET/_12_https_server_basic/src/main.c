// HTTPS Server Example for Zephyr with TLS
// Based on _11_http_server_basic with added TLS/HTTPS support
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/net_config.h>
#include <zephyr/logging/log.h>

// HTTPS server configuration
#define HTTPS_SERVER_PORT 4443
#define MAX_HTTPS_CLIENTS 4

LOG_MODULE_REGISTER(https_server_sample, LOG_LEVEL_DBG);

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

	LOG_DBG("Device info handler called, status: %d", status);

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
			LOG_ERR("Failed to format device info");
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

// Echo handler - echoes back the received data
// Client sends: GET /echo or POST /echo with body
// Server responds: Same body data back to client
static int echo_handler(struct http_client_ctx *client, enum http_data_status status,
						const struct http_request_ctx *request_ctx,
						struct http_response_ctx *response_ctx, void *user_data)
{
	enum http_method method = client->method;

	// Handle aborted transactions (connection closed by client)
	if (status == HTTP_SERVER_DATA_ABORTED)
	{
		LOG_DBG("Echo transaction aborted");
		return 0;
	}

	// Log received data
	if (request_ctx->data_len > 0)
	{
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
// TLS CERTIFICATE CONFIGURATION
// =============================================================================

#include "certificate.h"

static const sec_tag_t sec_tag_list_verify_none[] = {
	HTTP_SERVER_CERTIFICATE_TAG,
	PSK_TAG,
};

static void setup_tls(void)
{
	int err;

	LOG_INF("Setting up TLS credentials");

	err = tls_credential_add(HTTP_SERVER_CERTIFICATE_TAG,
							 TLS_CREDENTIAL_SERVER_CERTIFICATE,
							 server_certificate, sizeof(server_certificate));
	if (err < 0)
	{
		LOG_ERR("Failed to register server certificate: %d", err);
	}

	err = tls_credential_add(HTTP_SERVER_CERTIFICATE_TAG,
							 TLS_CREDENTIAL_PRIVATE_KEY,
							 private_key, sizeof(private_key));
	if (err < 0)
	{
		LOG_ERR("Failed to register private key: %d", err);
	}

	err = tls_credential_add(PSK_TAG,
							 TLS_CREDENTIAL_PSK,
							 psk,
							 sizeof(psk));
	if (err < 0)
	{
		LOG_ERR("Failed to register PSK: %d", err);
	}

	err = tls_credential_add(PSK_TAG,
							 TLS_CREDENTIAL_PSK_ID,
							 psk_id,
							 sizeof(psk_id) - 1);
	if (err < 0)
	{
		LOG_ERR("Failed to register PSK ID: %d", err);
	}
}

// =============================================================================
// HTTPS SERVICE CONFIGURATION
// =============================================================================

// Define the HTTPS service with TLS
// - Listens on all interfaces (NULL = bind to all)
// - Port: 4443
// - Max 4 simultaneous clients
// - Max 10 streams per client
// - Uses TLS with credentials
static uint16_t https_service_port = HTTPS_SERVER_PORT;
HTTPS_SERVICE_DEFINE(https_service, NULL, &https_service_port,
					 MAX_HTTPS_CLIENTS, 10, NULL, NULL, NULL, sec_tag_list_verify_none,
					 sizeof(sec_tag_list_verify_none));

// =============================================================================
// RESOURCE ROUTING
// =============================================================================

// Route "/" -> static HTML
HTTP_RESOURCE_DEFINE(index_resource, https_service, "/", &index_html_resource_detail);

// Route "/main.js" -> static JavaScript
HTTP_RESOURCE_DEFINE(main_js_resource, https_service, "/main.js", &main_js_resource_detail);

// Route "/device-info" -> dynamic endpoint (calls device_info_handler)
HTTP_RESOURCE_DEFINE(device_info_resource, https_service, "/device-info", &device_info_resource_detail);

// Route "/echo" -> dynamic endpoint (echoes back data)
HTTP_RESOURCE_DEFINE(echo_resource, https_service, "/echo", &echo_resource_detail);

// =============================================================================
// MAIN APPLICATION
// =============================================================================

int main(void)
{
	LOG_INF("Starting HTTPS Server on port %d", HTTPS_SERVER_PORT);
	LOG_INF("Available endpoints:");
	LOG_INF("  GET  /              -> HTML page");
	LOG_INF("  GET  /main.js       -> JavaScript");
	LOG_INF("  GET  /device-info   -> Device information (JSON)");
	LOG_INF("  GET/POST /echo      -> Echo server");

	// Setup TLS credentials before starting the server
	setup_tls();

	// Start the HTTPS server (blocking call)
	http_server_start();

	return 0;
}
