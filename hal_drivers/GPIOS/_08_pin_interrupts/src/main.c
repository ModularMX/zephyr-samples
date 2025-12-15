/**
 * @file main.c
 * @brief read a pin through interrupt
 * 
 * This sample application shows how to configure a pin as input and read its state
 * using interrupts. A button is connected to the input pin, when the button is pressed
 * an interrupt is triggered and the state of an led connected to an output pin is toggled
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* get the port, pin and flags from the device tree for both button and led */
const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET( DT_ALIAS(mybtn), gpios );
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET( DT_ALIAS(myled), gpios );

/* structure to store the callback data, mandatory to place in a global memory region */
static struct gpio_callback button_cb_data;

/*when the interrupt gets triggered, this function is called*/
static void button_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_pin_toggle_dt( &led );
}

int main( void )
{
    /* Configure button pin as input*/
    gpio_pin_configure_dt( &btn, GPIO_INPUT );
    /* Configure led pin as output */
    gpio_pin_configure_dt( &led, GPIO_OUTPUT );
    
    /* configure interrupt to be triggered when the button is pressed*/
    gpio_pin_interrupt_configure_dt( &btn, GPIO_INT_EDGE_FALLING );
    /* set the function that will be called during the ISR, and the pins 
    triggered the interrupt*/
    gpio_init_callback( &button_cb_data, button_cb, BIT(btn.pin) );
    
    /* set up the callback and enable it*/
    gpio_add_callback_dt( &btn, &button_cb_data );
    
    while(1)
    {
        /* 100ms Delay */
        k_msleep( 100 );
    }
    return 0;
}