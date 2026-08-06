/**
 * @file main.c
 * @brief Read one pin using port function
 * 
 * This sample application shows how to read a pin using the GPIO
 * driver API port functions. It reads the pin state every 100ms
 * and displays it using printk.
 * - On nrf54l15-DK it reads pin P0.13
 * - On STM32G0B1-DK it reads pin PC13
 * - On STM32H573I-DK it reads pin PC13
 * - On MCX_N9XX_EVK it reads pin P0.23
 * - On FRDM_MCXA156 it reads pin P1.07
 * - On POSIX it reads pin 0
 */
/* Include libraries */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


/*we are using pin functions with no access to device tree, so the onlyway to achieve 
hardware agnostic is use adefine with the pin to use way to achieve hardware agnostic 
is use adefine with the pin to use according to board in use (use of kconfig to define 
a pin is not recommended according to official Zephyr documentation)*/
#if defined(CONFIG_BOARD_NRF54L15DK)
    #define PORT_PIN 13
#elif defined(CONFIG_BOARD_STM32G0B1RE)
    #define PORT_PIN 13
#elif defined(CONFIG_BOARD_STM32H573I_DK)
    #define PORT_PIN 13
#elif defined(CONFIG_BOARD_MCX_N9XX_EVK)
    #define PORT_PIN 23
#elif defined(CONFIG_BOARD_FRDM_MCXA156)
    #define PORT_PIN 7
#elif defined(CONFIG_BOARD_NATIVE_SIM)
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