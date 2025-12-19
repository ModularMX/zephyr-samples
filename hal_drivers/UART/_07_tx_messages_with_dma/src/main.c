/**
 * @file main.c
 * @brief Tx a mesage using a DMA
 * 
 * This sample application shows how to transmit a message using the UART
 * peripheral with interrupts and DMA support. By default, the driver make use 
 * of a dma channel to buffer data to be transmitted, you must ensure the microcontroller
 * has a dma controller and a channel is available.
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

/* This is the function we are going to call durint the uart ISR */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);

int main( void )
{
    /* message to be transmitted */
    const uint8_t tx_buf[ ] = "Hola DMA uart Tx from DMA sending more bytes\r\n";
    struct k_sem tx_sem; 


    /* initialize the semaphore */
    k_sem_init( &tx_sem, 0, 1 );
    k_sem_reset( &tx_sem );
    /*make the semaphore available for the fisrt time*/
    k_sem_give( &tx_sem );

    /* set the callback function to be used on interrupts */
    uart_callback_set( uart_dev, uart_cb, &tx_sem );
    
    while(1)
    {
        /*Wait until the UART is free to transmit again, the function also 
        return a value of EBUSY if there is an ongoing transmission*/
        k_sem_take( &tx_sem, K_FOREVER );

        /*intiaite the transmition*/
        uart_tx( uart_dev, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
        /*we can also wait for the semaphore in here, to wait until the message has
        been transmitted*/
        
        /* wait for one second */
        k_msleep( 1000 );
    }

    return 0;
}

/* this callback will be called once all bytes have been transmitted*/
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    struct k_sem *sem = user_data;

	switch (evt->type) {
        case UART_TX_DONE:
            /*just singnaling the message is already transmitted*/
            k_sem_give(sem);
        break;
        case UART_TX_ABORTED:
        break;
        default:
        break;
    }
}