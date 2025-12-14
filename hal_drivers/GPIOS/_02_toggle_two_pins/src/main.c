/**
 * @file main.c
 * @brief Toggle one pin using port function
 * 
 * This sample application shows how to toggle more than one pin using the port function
 * gpio_port_toggle_bits(). The pins to toggle are defined based on the board in use.
 * - For the nrf54l15dk board, pin 9 and 7 from port 2 is toggled every 300ms.
 * - From the nucleo_g0b1re board, pin 0 and 1 from port C is toggled every 300ms.
 * - From the native_sim board, pin 0 and 1 from port 0 is toggled every 300ms.
 */
/* Include libraries */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


/*we are using pin functions with no access to device tree, so the only
way to achieve hardware agnostic is use adefine with the pin to use 
according to board in use*/
#ifdef CONFIG_SOC_NRF54L15
    #define PORT_PINA 9
    #define PORT_PINB 7
#elif CONFIG_SOC_STM32G0B1XX
    #define PORT_PINA 0
    #define PORT_PINB 1
#elif CONFIG_SOC_POSIX
    #define PORT_PINA 0
    #define PORT_PINB 1
#else
    #error "Define the pin to toggle for your board"
#endif 

/* Get the device descriptor for port with the pin connected */
const struct device *port = DEVICE_DT_GET( DT_ALIAS(gpio) );

int main( void )
{
    /* Configure pin as output */
    gpio_pin_configure( port, PORT_PINA, GPIO_OUTPUT);
    gpio_pin_configure( port, PORT_PINB, GPIO_OUTPUT);
    
    while(1)
    {
        /* Toggle two pins from port using a port function */
        gpio_port_toggle_bits( port, (1<<PORT_PINA) | (1<<PORT_PINB) );
        /* 300ms Delay */
        k_msleep( 300 );
    }
    return 0;
}