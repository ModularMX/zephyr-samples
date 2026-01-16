#include <zephyr/kernel.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(eth_basic, LOG_LEVEL_INF);

int main(void)
{
    struct net_if *iface;

    LOG_INF("=== STM32H573 Ethernet Basic Connection Test ===");

    /* Get the default network interface */
    iface = net_if_get_default();
    if (!iface)
    {
        LOG_ERR("ERROR: No network interface found!");
        return 1;
    }

    LOG_INF("Network interface found: %p", iface);

    /* Get and display MAC address */
    const struct net_linkaddr *linkaddr = net_if_get_link_addr(iface);
    if (linkaddr && linkaddr->len == 6)
    {
        LOG_INF("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
                linkaddr->addr[0], linkaddr->addr[1], linkaddr->addr[2],
                linkaddr->addr[3], linkaddr->addr[4], linkaddr->addr[5]);
    }
    else
    {
        LOG_WRN("Could not read MAC address");
    }

    /* Get interface type */
    LOG_INF("Interface type: %s", iface->if_dev->dev->name);

    /* Display interface state */
    LOG_INF("Interface is %s", net_if_is_up(iface) ? "UP" : "DOWN");

    LOG_INF("Ethernet test complete. Waiting...");

    while (1)
    {
        k_sleep(K_SECONDS(10));
    }

    return 0;
}
