/**
 * @file main.c
 * @brief Toggle two pins in the same port
 * 
 * This sample application shows how to toggle more than one pin using the port function
 * gpio_port_toggle_bits(). The pins to toggle are defined based on the board in use.
 * - For the nrf54l15dk board, pin 9 and 7 from port 2 is toggled every 300ms.
 * - From the nucleo_g0b1re board, pin 0 and 1 from port C is toggled every 300ms.
 * - From the stm32h573i_dk board, pin 4 and 1 from port F is toggled every 300ms.
 * - From the native_sim board, pin 0 and 1 from port 0 is toggled every 300ms.
 */
/* Include libraries */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


/*we are using pin functions with no access to device tree, so the onlyway to achieve 
hardware agnostic is use adefine with the pin to use way to achieve hardware agnostic 
is use adefine with the pin to use according to board in use (use of kconfig to define 
a pin is not recommended according to official Zephyr documentation)*/
#if defined(CONFIG_BOARD_NRF54L15DK)
    #define PORT_PINA 9
    #define PORT_PINB 7
#elif defined(CONFIG_BOARD_NUCLEO_G0B1RE)
    #define PORT_PINA 0
    #define PORT_PINB 1
#elif defined(CONFIG_BOARD_STM32H573I_DK)
    #define PORT_PINA 4
    #define PORT_PINB 1
#elif defined(CONFIG_BOARD_NATIVE_SIM)
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
        gpio_port_toggle_bits( port, BIT(PORT_PINA) | BIT(PORT_PINB) );
        /* 300ms Delay */
        k_msleep( 300 );
    }
    return 0;
}