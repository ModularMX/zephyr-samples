/**
 * @file main.c
 * @brief Initialize one pin using DTC hogs
 * 
 * This sample application shows how to configure a pin as output
 * using a hog in the device tree. The pin is toggled every 300ms
 * in the main loop using a port function.
 */
/* Include libraries */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


/*we are using pin functions with no access to device tree, so the only
way to achieve hardware agnostic is use adefine with the pin to use 
according to board in use*/
#ifdef CONFIG_SOC_NRF54L15
    #define PORT_PIN 9
#elif CONFIG_SOC_STM32G0B1XX
    #define PORT_PIN 5
#elif CONFIG_SOC_POSIX
    #define PORT_PIN 0
#else
    #error "Define the pin to toggle for your board"
#endif 

/* Get the device descriptor for port with the pin connected */
const struct device *port = DEVICE_DT_GET( DT_ALIAS(gpio) );

int main( void )
{
    /* No need to call any configuration function */

    while(1)
    {
        /* Toggle pin 9 from port 2 using a port function */
        gpio_pin_toggle( port, PORT_PIN );
        /* 300ms Delay */
        k_msleep( 300 );
    }
    return 0;
}