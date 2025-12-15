/**
 * @file main.c
 * @brief pass data to GPIO interrupt callback example
 * 
 * This sample application shows how to pass data to a GPIO interrupt callback
 * using a structure that contains both the gpio_callback structure and the data
 * to be passed to the callback function. In this case, the data is a reference
 * to an led gpio_dt_spec structure that is used to toggle the led state when
 * the button interrupt is triggered.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* structure to store the callback data and the led reference to pass to callback*/
struct button_int_data {
    struct gpio_callback button_cb_data;
    struct gpio_dt_spec *led;
};

/* get the port, pin and flags from the device tree for both button and led */
const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET( DT_ALIAS(mybtn), gpios );
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET( DT_ALIAS(myled), gpios );

/* structure to store the callback data, mandatory to place in a global memory region */
static struct button_int_data button_int;

/*when the interrupt gets triggered, this function is called*/
static void button_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    /*get from the cb pointer the data embedded in the button_int_data structure*/
    struct button_int_data *data = CONTAINER_OF(cb, struct button_int_data, button_cb_data);
    /*toggle the led state*/
    gpio_pin_toggle_dt( data->led );
}

int main( void )
{
    /* Configure button pin as input*/
    gpio_pin_configure_dt( &btn, GPIO_INPUT );
    /* Configure led pin as output */
    gpio_pin_configure_dt( &led, GPIO_OUTPUT );
    
    /* configure interrupt to be triggered when the button is pressed*/
    gpio_pin_interrupt_configure_dt( &btn, GPIO_INT_EDGE_FALLING );
    
    /*set the led reference to pass to interrupt callback*/
    button_int.led = (struct gpio_dt_spec *)&led;
    /* set the function that will be called during the ISR, and the pins 
    triggered the interrupt*/
    gpio_init_callback( &button_int.button_cb_data, button_cb, BIT(btn.pin) );
    
    /* set up the callback and enable it*/
    gpio_add_callback_dt( &btn, &button_int.button_cb_data );
    
    while(1)
    {
        /* 100ms Delay */
        k_msleep( 100 );
    }
    return 0;
}