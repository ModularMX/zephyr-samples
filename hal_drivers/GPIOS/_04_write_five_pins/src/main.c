/**
 * @file main.c
 * @brief Write a 5 bits value to a GPIO port
 * 
 * This sample application shows how to write a value to a GPIO port using
 * gpio_port_clear_bits and gpio_port_set_bits functions.
 * NOTE: This code assumes that the first 5 pins of the selected port are
 * availabe to be used as GPIOs.
 * - On nrf54l15dk we are using port1
 * - On nucleo_g0b1re we are using portc
 * - On native_sim we are using port0
 */
/* Include libraries */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Get the device descriptor for port with the pin connected */
const struct device *port = DEVICE_DT_GET( DT_ALIAS(gpio) );

int main( void )
{
    /* Set pins 0 - 4 from port "port" as output, one by one, there isn't a
    function to configure more than one pin */
    for(uint8_t pin = 0; pin < 5; pin++){
        gpio_pin_configure( port, pin, GPIO_OUTPUT );
    }
    
    while(1)
    {
        /* Write the value 0x15 on port 0 */
        gpio_port_clear_bits( port, 0xFF );
        gpio_port_set_bits( port, 0x15 );
        k_msleep( 300 );
        
        /* Write the value 0x0A on port 0 */
        gpio_port_clear_bits( port, 0xFF );
        gpio_port_set_bits( port, 0x0A );
        k_msleep( 300 );
    }
    return 0;
}