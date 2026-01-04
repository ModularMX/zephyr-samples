/*
 * LED driver demo using DT.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/led.h>

/* Get LED0 from devicetree */
static const struct led_dt_spec led = LED_DT_SPEC_GET(DT_ALIAS(my_led));

int main(void)
{
    /* Check that the LED device is ready */
    if (!led_is_ready_dt(&led))
    {
        return 0;
    }

    while (1)
    {
        led_on_dt(&led);
        k_sleep(K_MSEC(500));

        led_off_dt(&led);
        k_sleep(K_MSEC(500));
    }

    return 0;
}
