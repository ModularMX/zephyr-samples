/**
 * @file main.c
 * @brief Read one pin using port function
 * 
 * This sample application shows how to read a pin using the GPIO
 * driver API port functions. It reads the pin state every 100ms
 * and displays it using printk.
 * - On nrf54l15-DK it reads pin P0.13
 * - On STM32G0B1-DK it reads pin PC13
 * - On POSIX it reads pin 0
 */
/* Include libraries */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


/*we are using pin functions with no access to device tree, so the only
way to achieve hardware agnostic is use adefine with the pin to use 
according to board in use*/
#ifdef CONFIG_SOC_NRF54L15
    #define PORT_PIN 13
#elif CONFIG_SOC_STM32G0B1XX
    #define PORT_PIN 13
#elif CONFIG_SOC_POSIX
    #define PORT_PIN 0
#else
    #error "Define the pin to toggle for your board"
#endif 

/* Get the device descriptor for port with the pin connected */
const struct device *port = DEVICE_DT_GET( DT_ALIAS(gpio) );

int main( void )
{
    int pin_state;
    /* Configure pin as output with an active pull-up */
    gpio_pin_configure( port, PORT_PIN, (GPIO_INPUT | GPIO_PULL_UP) );
    
    while(1)
    {
        /* read the pin every and diplay its state */
        pin_state = gpio_pin_get( port, PORT_PIN );
        printk( "Pin state: %d \n", pin_state );
        /* 100ms Delay */
        k_msleep( 100 );
    }
    return 0;
}