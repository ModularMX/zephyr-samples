/*
 * LED PWM Brightness Control Example
 * Demonstrates smooth LED fading using PWM brightness control
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/sys/printk.h>

#define LED_PWM_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds)

#define FADE_DELAY_MS 20    // Delay between brightness steps (ms)
#define FADE_STEP     2     // Brightness increment/decrement step

int main(void)
{
    const struct device *pwm_leds = DEVICE_DT_GET(LED_PWM_NODE_ID);
    uint8_t brightness = 0;
    int8_t direction = FADE_STEP;  // Positive = fade up, negative = fade down
    int ret;

    if (!device_is_ready(pwm_leds))
    {
        printk("PWM LED device not ready\n");
        return 0;
    }

    printk("Starting PWM LED brightness control (fade effect)...\n");
    printk("Fading from 0%% to 100%% and back\n");

    while (1)
    {
        // Set LED brightness (0-100%)
        ret = led_set_brightness(pwm_leds, 0, brightness);
        
        if (ret != 0)
        {
            printk("led_set_brightness failed: %d\n", ret);
            return 0;
        }

        // Update brightness for next iteration
        brightness += direction;

        // Reverse direction at limits
        if (brightness == 0 || brightness == 100)
        {
            direction = -direction;
            
            if (brightness == 0)
            {
                printk("Faded to 0%% (OFF)\n");
            }
            else
            {
                printk("Faded to 100%% (MAX)\n");
            }
        }

        // Delay between steps for smooth fading
        k_sleep(K_MSEC(FADE_DELAY_MS));
    }

    return 0;
}

