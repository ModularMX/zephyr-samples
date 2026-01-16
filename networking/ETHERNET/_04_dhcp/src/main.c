/* Networking DHCPv4 client with PHY monitoring - HYBRID APPROACH */

/*
 * Copyright (c) 2017 ARM Ltd.
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(eth_dhcp_client, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/linker/sections.h>
#include <errno.h>
#include <stdio.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/net_mgmt.h>

#define DHCP_OPTION_NTP (42)

static uint8_t ntp_server[4];

static struct net_mgmt_event_callback mgmt_cb;
static struct net_dhcpv4_option_callback dhcp_cb;

/**
 * @brief Register PHY link state callback for monitoring
 */
static void register_phy_callback(struct net_if *iface)
{
    const struct device *phy_dev = net_eth_get_phy(iface);

    if (!phy_dev || !device_is_ready(phy_dev))
    {
        LOG_WRN("PHY device not available for link monitoring");
        return;
    }

    LOG_DBG("PHY device: %s (monitoring only, not interfering with DHCP)",
            phy_dev->name);
    /* Don't register callback - let DHCP work without interference */
}

static void start_dhcpv4_client(struct net_if *iface, void *user_data)
{
    ARG_UNUSED(user_data);

    LOG_INF("=== DHCP CLIENT START ===");
    LOG_INF("Starting DHCP on %s: index=%d",
            net_if_get_device(iface)->name,
            net_if_get_by_iface(iface));

    /* Just note PHY info, don't register callbacks */
    register_phy_callback(iface);

    net_dhcpv4_start(iface);
}

static void handler(struct net_mgmt_event_callback *cb,
                    uint64_t mgmt_event,
                    struct net_if *iface)
{
    int i = 0;

    if (mgmt_event != NET_EVENT_IPV4_ADDR_ADD)
    {
        return;
    }

    LOG_INF("=== IPV4 ADDRESS ADDED (DHCP) ===");

    for (i = 0; i < NET_IF_MAX_IPV4_ADDR; i++)
    {
        char buf[NET_IPV4_ADDR_LEN];

        if (iface->config.ip.ipv4->unicast[i].ipv4.addr_type !=
            NET_ADDR_DHCP)
        {
            continue;
        }

        LOG_INF("   Address[%d]: %s", net_if_get_by_iface(iface),
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
                              buf, sizeof(buf)));
        LOG_INF("    Subnet[%d]: %s", net_if_get_by_iface(iface),
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->unicast[i].netmask,
                              buf, sizeof(buf)));
        LOG_INF("    Router[%d]: %s", net_if_get_by_iface(iface),
                net_addr_ntop(NET_AF_INET,
                              &iface->config.ip.ipv4->gw,
                              buf, sizeof(buf)));
        LOG_INF("Lease time[%d]: %u seconds (%u days)",
                net_if_get_by_iface(iface),
                iface->config.dhcpv4.lease_time,
                iface->config.dhcpv4.lease_time / 86400);
    }
}

static void option_handler(struct net_dhcpv4_option_callback *cb,
                           size_t length,
                           enum net_dhcpv4_msg_type msg_type,
                           struct net_if *iface)
{
    char buf[NET_IPV4_ADDR_LEN];

    LOG_INF("DHCP Option %d: %s", cb->option,
            net_addr_ntop(NET_AF_INET, cb->data, buf, sizeof(buf)));
}

int main(void)
{
    LOG_INF("=== STM32H573 DHCP CLIENT (HYBRID: Official + PHY Monitoring) ===");

    net_mgmt_init_event_callback(&mgmt_cb, handler, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&mgmt_cb);

    net_dhcpv4_init_option_callback(&dhcp_cb, option_handler,
                                    DHCP_OPTION_NTP, ntp_server,
                                    sizeof(ntp_server));

    net_dhcpv4_add_option_callback(&dhcp_cb);

    LOG_INF("Registering DHCP on all interfaces...");
    net_if_foreach(start_dhcpv4_client, NULL);

    LOG_INF("DHCP started, waiting for IP assignment...");
    return 0;
}
