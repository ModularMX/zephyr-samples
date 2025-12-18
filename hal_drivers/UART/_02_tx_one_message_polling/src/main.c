/**
 * @file main.c
 * @brief message transmission using UART polling sample application
 * 
 * This sample application shows how to send a message through a UART in 
 * polling mode. The message is sent one character at a time using the
 * uart_poll_out() function.
 * 
 * NOTE: By default one uart is always configured to be used for the printk()
 * function. Just take a look at your board's .dts file to see which uart is
 * used for that purpose and avoid using it in this sample.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

/* Get the device tree name for the uart port using an alias, 
as it is defined in the device tree file*/
const struct device *uart_dev = DEVICE_DT_GET( DT_ALIAS( uart ) );

/* Set an array with the message */
const char msg[] = "Hello from UART\n";

int main( void )
{
    while(1)
    {
        /* Send characters one by one with uart_poll_out() */
        for( uint8_t i = 0; i < strlen( msg ); i++){
            uart_poll_out( uart_dev, msg[i] );
        }
        /* Just to keep our main thread */
        k_msleep( 1000 );
    }

    return 0;
}
