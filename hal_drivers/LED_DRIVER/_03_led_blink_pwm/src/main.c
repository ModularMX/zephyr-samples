/*
 * LED PWM Blink Example
 * Demonstrates hardware-based LED blinking using PWM LEDs
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/sys/printk.h>

#define LED_PWM_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds)

int main(void)
{
    // Get PWM LED device from devicetree
    const struct device *pwm_leds = DEVICE_DT_GET(LED_PWM_NODE_ID);

    // Ensure driver is ready
    if (!device_is_ready(pwm_leds))
    {
        printk("PWM LED device not ready\n");
        return 0;
    }

    printk("Starting PWM LED blink...\n");

    /*
     * Hardware blink LED index 0:
     *  - 1000 ms ON
     *  - 100 ms OFF
     * 
     * With PWM LEDs, led_blink() uses hardware PWM to control the LED,
     * no CPU intervention needed once configured.
     */
    int ret = led_blink(pwm_leds, 0, 1000, 100);

    if (ret != 0)
    {
        printk("led_blink failed: %d\n", ret);
        return 0;
    }

    printk("LED blinking in hardware, CPU is free!\n");

    // LED blinks autonomously via PWM hardware
    while (1)
    {
        k_sleep(K_SECONDS(1));
    }

    return 0;
}

