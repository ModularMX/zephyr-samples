/**
 * @file main.c
 * @brief Tx a mesage using a DMA and net buffer
 * 
 * This sample application shows how to transmit multiple messages using UART
 * with DMA support and net buffers to manage the memory for the messages.
 * This method will allow the application to buffer multiple messages to be
 * transmitted one after the other without having to wait for each transmission
 * 
 * NOTE: By default one uart is always configured to be used for the printk()
 * function. Just take a look at your board's .dts file to see which uart is
 * used for that purpose and avoid using it in this sample.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/net_buf.h>

#define MAX_BUFFERS 3
#define MAX_BUFF_SIZE 64

struct serial_tx {
    struct k_sem tx_sem;
    struct k_fifo tx_queue; 
    struct net_buf *tx_buf1;
    struct net_buf *tx_buf2;
    struct net_buf *tx_buf3;
    struct net_buf *ref_buff;
};

/* Get the device tree name for the uart port using an alias, 
as it is defined in the device tree file*/
const struct device *uart_dev = DEVICE_DT_GET( DT_ALIAS( uart ) );

/* This is the function we are going to call durint the uart ISR */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);

/* Define a net buffer pool to allocate buffers for transmission */
NET_BUF_POOL_DEFINE(tx_net_pool, MAX_BUFFERS, MAX_BUFF_SIZE, 0, NULL);

struct serial_tx serial;
/* message to be transmitted */
const uint8_t message1[ ] = "First: Hola DMA uart Tx from DMA sending more\r\n";
const uint8_t message2[ ] = "Second: Este s otro mensaje muy difernete\r\n";
const uint8_t message3[ ] = "Third: Aqui hay uno mas para mandar seguido\r\n";

int main( void )
{   
    /* initialize the semaphore */
    k_sem_init( &serial.tx_sem, 0, 1 );
    k_sem_reset( &serial.tx_sem );
    /*make the semaphore available for the fisrt time*/
    k_sem_give( &serial.tx_sem );
    
    /* set the callback function to be used on interrupts */
    uart_callback_set( uart_dev, uart_cb, &serial.tx_sem );
    
    while(1)
    {
        /*We can use a sempahore to wait until the UART is free to transmit again
        the function also return a value of EBUSY if there is an ongoing transmission*/
        k_sem_take( &serial.tx_sem, K_FOREVER );

        /*this is the first message to send*/
        serial.tx_buf1 = net_buf_alloc( &tx_net_pool, K_NO_WAIT );
        /* Populate it with data */
        memcpy( serial.tx_buf1->data, message1, sizeof(message1) );
        net_buf_add(serial.tx_buf1, sizeof(message1));

        /*second message to send */
        serial.tx_buf2 = net_buf_alloc( &tx_net_pool, K_NO_WAIT );
        /* Populate it with data */
        memcpy( serial.tx_buf2->data, message2, sizeof(message2) );
        net_buf_add(serial.tx_buf2, sizeof(message2));
        
        /*third message to send */
        serial.tx_buf3 = net_buf_alloc( &tx_net_pool, K_NO_WAIT );
        /* Populate it with data */
        memcpy( serial.tx_buf3->data, message3, sizeof(message3) );
        net_buf_add(serial.tx_buf3, sizeof(message3));

        /*queue messages second and third*/
        k_fifo_put(&serial.tx_queue, serial.tx_buf2);
        k_fifo_put(&serial.tx_queue, serial.tx_buf3);
        
        /*intiaite the transmition of the first message*/
        serial.ref_buff = serial.tx_buf1;
        uart_tx( uart_dev, serial.ref_buff->data, serial.ref_buff->len, SYS_FOREVER_US);
        
        /* wait for one second */
        k_msleep( 1000 );
    }

    return 0;
}

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    struct serial_tx *serial = user_data;

	switch (evt->type) {
        case UART_TX_DONE:
            /*release memory from the pool*/
            net_buf_unref(serial->ref_buff);
            /*lets find out if there are more messages*/
            serial->ref_buff = k_fifo_get(&serial->tx_queue, K_NO_WAIT);
            if(serial->ref_buff != NULL){
                /*send next message*/
                uart_tx( dev, serial->ref_buff->data, serial->ref_buff->len, SYS_FOREVER_US);
            }
            else{
                /*no more messages to send, release the semaphore*/
                k_sem_give(&serial->tx_sem);
            }
        break;
        case UART_TX_ABORTED:
        break;
        default:
        break;
    }
}