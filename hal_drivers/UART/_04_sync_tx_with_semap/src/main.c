/**
 * @file main.c
 * @brief UART synchronous transmission using interrupts and semaphores
 * 
 * This sample application shows how to use the UART driver to transmit data
 * using interrupts and semaphores for synchronization. The application initializes 
 * the UART device, sets up an interrupt-driven transmission, and uses a semaphore
 * to signal the completion of the transmission.
 * 
 * NOTE: By default one uart is always configured to be used for the printk()
 * function. Just take a look at your board's .dts file to see which uart is
 * used for that purpose and avoid using it in this sample.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

/* define a structure to control data to transmit */
struct uart_msg{
    uint8_t tx_buf[50];
    uint8_t tx_count;
    uint8_t tx_len;
    struct k_sem tx_done;
};

/* string to send trough serial port*/
const uint8_t message[] = "Hello UART transmition interrupts example!\r\n";

/* Get the device tree name for the uart port using an alias, 
as it is defined in the device tree file*/
const struct device *serial = DEVICE_DT_GET( DT_ALIAS( uart ) );

/* This is the function we are going to call durint the uart ISR */
void uart_cb( const struct device *dev, void* user_data );

int main( void )
{
    struct uart_msg tx_message;

    /* init the control structure with the message to send */
    strcpy( tx_message.tx_buf, message );
    tx_message.tx_len = strlen( message );
    tx_message.tx_count = tx_message.tx_len;

    /* initialize the semaphore */
    k_sem_init( &tx_message.tx_done, 0, 1 );
    k_sem_reset( &tx_message.tx_done );
    /*make the semaphire available for the fisrt time*/
    k_sem_give( &tx_message.tx_done );
    /* set the callback function to be used on interrupts and the message to send */
    uart_irq_callback_user_data_set( serial, uart_cb, (void*)&tx_message );

    while(1)
    {
        /*before trying to transmit make sure the uart is free, otherwise
        wait until it is, it will always be because our 1 secod delay,
        but it is here just to show you how to communicate the ISR with the thread*/
        k_sem_take( &tx_message.tx_done, K_FOREVER );
        /* since we are going to send the same message we only need to 
        enable the Tx interrupts again and the transmition counter */
        tx_message.tx_count = tx_message.tx_len;
        uart_irq_tx_enable( serial );
        /*you can also try to take the semaphore after initiate the transmission
        to wait until is done, but maybe you don't want to waste time*/

        /* wait for one second */
        k_msleep( 1000 );
    }

    return 0;
}

void uart_cb( const struct device *dev, void* user_data )
{
    uint8_t index;
    int sended;
    struct uart_msg *message = user_data;

    /* acknowledge the interrupt */
    if(uart_irq_update(dev) && uart_irq_is_pending(dev)){
        /* check if it was a tx interrupt */
        if(uart_irq_tx_ready(dev)){
            if(message->tx_count != 0){
                /* determine from what index we nned to send the message*/
                index = message->tx_len - message->tx_count;
                /* send as many bytes as possible, this will depend on microcontroller 
                internal UART FIFO size, read the corresponding datasheet */
                sended = uart_fifo_fill( dev, &message->tx_buf[index], message->tx_count );
                /* discount the actual bytes seneded from the counter */
                message->tx_count -= (uint8_t)sended;
            }
        }

        if(message->tx_count == 0){
            /* All bytes sent = disable TX interrupt */
            uart_irq_tx_disable(dev);
            /* give the semaphore to indicate that the message was completely sent */
            k_sem_give( &message->tx_done );
        }
    }
}
