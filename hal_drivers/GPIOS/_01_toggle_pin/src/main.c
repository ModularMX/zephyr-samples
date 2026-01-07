/**
 * @file main.c
 * @brief Toggle one pin using port function
 *
 * This sample application shows how to toggle a pin using the port function
 * gpio_pin_toggle(). The pin to toggle is defined based on the board in use.
 * - For the nrf54l15dk board, pin 9 from port 2 is toggled.
 * - For the nucleo_g0b1re board, pin 5 from port A is toggled.
 * - For the stm32h573i_dk board, pin 4 from port F (blue LED) is toggled.
 * - For the native_sim board, pin 0 from port 0 is toggled.
 *
 * The toggle delay can be configured at build time (default: 300ms).
 * Example: west build -b <board> -- -DTOGGLE_DELAY_MS=500
 */
/* Include libraries */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


/*we are using pin functions with no access to device tree, so the only
way to achieve hardware agnostic is use adefine with the pin to use 
according to board in use*/
#ifdef CONFIG_SOC_NRF54L15
    #define PORT_PIN 9
#elif defined(CONFIG_SOC_STM32G0B1XX)
#define PORT_PIN 5
#elif defined(CONFIG_BOARD_STM32H573I_DK)
/* Blue LED is on GPIOF pin 4 on stm32h573i_dk */
#define PORT_PIN 4
#elif defined(CONFIG_SOC_POSIX)
#define PORT_PIN 0
#else
    #error "Define the pin to toggle for your board"
#endif 

/* Get the device descriptor for port with the pin connected */
const struct device *port = DEVICE_DT_GET( DT_ALIAS(gpio) );

int main( void )
{
    /* Configure pin as output */
    gpio_pin_configure( port, PORT_PIN, GPIO_OUTPUT);
    
    while(1)
    {
        /* Toggle pin from port using a port function */
        gpio_pin_toggle( port, PORT_PIN );
        /* Delay configured at build time (default: 300ms) */
        k_msleep(TOGGLE_DELAY_MS);
    }
    return 0;
}