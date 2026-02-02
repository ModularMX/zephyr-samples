/*
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>

#include "mqtt_client.h"
#include "mqtt_config.h"
#include "device.h"

/* MQTT client struct */
static struct mqtt_client client_ctx;

/* MQTT publish work item */
struct k_work_delayable mqtt_publish_work;

/* Network management callback */
static struct net_mgmt_event_callback mgmt_cb;

/* Network connection semaphore */
K_SEM_DEFINE(net_conn_sem, 0, 1);

/**
 * Network event handler - triggered when IPv4 address is added/removed
 * This works with net_config which handles DHCP automatically
 */
static void net_event_handler(struct net_mgmt_event_callback *cb,
							  uint64_t mgmt_event, struct net_if *iface)
{
	if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD)
	{
		k_sem_give(&net_conn_sem);
		printk("IPv4 address assigned - network ready!\n");
	}
}

/** Print the device's MAC address to console */
void log_mac_addr(struct net_if *iface)
{
	struct net_linkaddr *mac;

	mac = net_if_get_link_addr(iface);

	printk("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
		   mac->addr[0], mac->addr[1], mac->addr[2],
		   mac->addr[3], mac->addr[4], mac->addr[5]);
}

/** The system work queue is used to handle periodic MQTT publishing.
 *  Work queuing begins when the MQTT connection is established.
 *  Use MQTT_PUBLISH_INTERVAL to set the publish frequency.
 */

static void publish_work_handler(struct k_work *work)
{
	int rc;

	if (mqtt_connected)
	{
		rc = app_mqtt_publish(&client_ctx);
		if (rc != 0)
		{
			printk("MQTT Publish failed [%d]\n", rc);
		}
		k_work_reschedule(&mqtt_publish_work,
						  K_SECONDS(MQTT_PUBLISH_INTERVAL));
	}
	else
	{
		k_work_cancel_delayable(&mqtt_publish_work);
	}
}

int main(void)
{
	int rc;
	struct net_if *iface;

	devices_ready();

	iface = net_if_get_default();
	if (iface == NULL)
	{
		printk("No network interface configured\n");
		return -ENETDOWN;
	}

	log_mac_addr(iface);

	/* Register callback to be notified when IPv4 address is assigned by DHCP.
	 * net_config (enabled via CONFIG_NET_CONFIG_SETTINGS=y) handles DHCP automatically,
	 * and will trigger NET_EVENT_IPV4_ADDR_ADD when IP is received.
	 */
	net_mgmt_init_event_callback(&mgmt_cb, net_event_handler, NET_EVENT_IPV4_ADDR_ADD);
	net_mgmt_add_event_callback(&mgmt_cb);

	printk("Waiting for network configuration (DHCP)...\n");

	/* Check if we already have an IP address (DHCP may have completed before main()) */
	if (!net_if_ipv4_get_global_addr(iface, NET_ADDR_PREFERRED))
	{
		/* Wait for IPv4 address assignment from DHCP */
		while (k_sem_take(&net_conn_sem, K_MSEC(5000)) != 0)
		{
			printk("Still waiting for IP address...\n");
		}
	}
	else
	{
		printk("IPv4 address already assigned!\n");
	}

	printk("Network ready, initializing MQTT...\n");

	rc = app_mqtt_init(&client_ctx);
	if (rc != 0)
	{
		printk("MQTT Init failed [%d]\n", rc);
		return rc;
	}

	/* Initialise MQTT publish work item */
	k_work_init_delayable(&mqtt_publish_work, publish_work_handler);

	/* Thread main loop */
	while (1)
	{
		/* Block until MQTT connection is up */
		app_mqtt_connect(&client_ctx);

		/* We are now connected, begin queueing periodic MQTT publishes */
		k_work_reschedule(&mqtt_publish_work,
						  K_SECONDS(MQTT_PUBLISH_INTERVAL));

		/* Handle MQTT inputs and connection */
		app_mqtt_run(&client_ctx);
	}

	return rc;
}
