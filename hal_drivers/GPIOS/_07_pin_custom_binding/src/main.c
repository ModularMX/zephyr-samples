/**
 * @file main.c
 * @brief read a pin and write its value to another pin
 * 
 * This sample application shows how to read a pin configured as input
 * and write its value to another pin configured as output, the sample 
 * use a custom binding in order to use _dt version of the GPIO functions.
 * Read the overlay files to see pins has been used according to the board.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* get the port, pin and flags from the device tree for both button and led */
const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET( DT_ALIAS(mybtn), gpios );
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET( DT_ALIAS(myled), gpios );

int main( void )
{
    int state;

    /* Configure button pin as input*/
    gpio_pin_configure_dt( &btn, GPIO_INPUT );
    /* Configure led pin as output */
    gpio_pin_configure_dt( &led, GPIO_OUTPUT );

    while(1)
    {
        /* Get logical level of button */
        state = gpio_pin_get_dt( &btn );
        /* Set logical level of led */
        gpio_pin_set_dt( &led, state );
        /* 100ms Delay */
        k_msleep( 100 );
    }
    return 0;
}