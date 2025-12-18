/**
 * @file main.c
 * @brief UART reception using interrupts sample application
 * 
 * This sample application shows how to configure a UART peripheral to receive data
 * using interrupts. The application configures a callback function to be called
 * when data is received. The callback function reads the data from the UART FIFO
 * and stores it in a buffer until a carriage return character ('\r') is received,
 * indicating the end of a message. The main loop waits for a complete message and
 * then prints it to the console.
 * 
 * NOTE: By default one uart is always configured to be used for the printk()
 * function. Just take a look at your board's .dts file to see which uart is
 * used for that purpose and avoid using it in this sample.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

/* define a structure to control data to transmit */
struct uart_msg{
    uint8_t rx_buf[50];
    uint8_t rx_count;
    uint8_t rx_len;
    bool rx_flag;
};

/* Get the device tree name for the uart port using an alias, 
as it is defined in the device tree file*/
const struct device *serial = DEVICE_DT_GET( DT_ALIAS( uart ) );

/* This is the function we are going to call durint the uart ISR */
void uart_cb( const struct device *dev, void* user_data );

int main( void )
{
    struct uart_msg rx_message;

    /* set intial values for our reception buffer */
    rx_message.rx_len = sizeof( rx_message.rx_buf );
    rx_message.rx_count = 0;
    rx_message.rx_flag = false;

    /* set the callback function to be used on interrupts and the buffer 
    to store the bytes received, also enable interrupts*/
    uart_irq_callback_user_data_set( serial, uart_cb, (void*)&rx_message );
    uart_irq_rx_enable( serial );

    while(1)
    {
        /* Let's poll until a '\r' terminated message is received. We should disable 
        interrupt at this point or use a semaphore because we are accessing a shared 
        memory region between the ISR*/
        if( rx_message.rx_flag == true ){
            rx_message.rx_flag = false;
            /* print the message received */
            printk( "mesage received: %s\n", rx_message.rx_buf );
        }
        /* wait for 50ms */
        k_msleep( 50 );
    }

    return 0;
}

void uart_cb( const struct device *dev, void* user_data )
{
    struct uart_msg *message = user_data;
    int rx_bytes;

    /* acknowledge the interrupt */
    if(uart_irq_update(dev) && uart_irq_is_pending(dev)){
        /* check if it was a rx interrupt */
        if(uart_irq_rx_ready(dev)){
            /* read all bytes received, how many bytes were read will depend
            on the microcontroller's UART FIFO size nad its configuration */
            rx_bytes = uart_fifo_read( dev, &message->rx_buf[message->rx_count], message->rx_len );
            /* process the received bytes one by one and check if the '\r' character was received */
            for( int i = 0; i < rx_bytes; i++ ){
                if( message->rx_buf[message->rx_count + i] == '\r' ){
                    /* if we received a carriage return, we can consider the message complete */
                    message->rx_flag = true;
                    message->rx_buf[message->rx_count + i] = '\0'; // null terminate the string
                    message->rx_count = 0;
                }
                else{
                    /* increment the count of bytes received */
                    message->rx_count++;
                    /* avoid buffer overflow */
                    if( message->rx_count >= message->rx_len ){
                        message->rx_count = 0;
                    }
                }
            }
        }
    }
}