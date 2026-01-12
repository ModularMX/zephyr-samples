/*
 * LED driver demo.
 * Minimal C side; see board overlays for LED mapping details.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>

// Pick LED index; consult the overlay for each board's mapping
// STM32H573I-DK: 0-3 board LEDs, 4-5 overlay LEDs
// XIAO ESP32C6: 0 = GPIO20 (overlay)
#define LED_INDEX 4

int main(void)
{
    // Get LED device from devicetree ("leds" node)
    const struct device *leds = device_get_binding("leds");

    // Ensure driver is ready before use
    if (!device_is_ready(leds))
    {
        return 0;
    }

    // Blink at 500 ms interval
    while (1)
    {
        led_on(leds, LED_INDEX);
        k_sleep(K_MSEC(500));
        led_off(leds, LED_INDEX);
        k_sleep(K_MSEC(500));
    }

    return 0;
}
