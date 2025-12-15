/**
 * @file main.c
 * @brief Rotate one pin through first five pins of a port
 * 
 * This sample application shows how to rotate a high logic level
 * through the first five pins of a port using gpio_port_set_masked 
 * port functions.
 * NOTE: This code assumes that the first 5 pins of the selected port are
 * availabe to be used as GPIOs.
 */
/* Include libraries */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Get the device descriptor for port with the pin connected */
const struct device *port = DEVICE_DT_GET( DT_ALIAS(gpio) );

int main( void )
{
    /* No need to call any configuration function */

    while(1)
    {
        for(uint8_t pin = 0; pin < 5; pin++){
            /* write a new value into the first 5 bits */
            gpio_port_set_masked( port, 0x0F, ( 1 << pin ) );
            k_msleep(300);
        }
    }
    return 0;
}