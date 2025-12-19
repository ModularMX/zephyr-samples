/**
 * @file main.c
 * @brief UART RX messages with DMA sample application
 * 
 * This sample application shows how to use the UART driver with DMA to receive
 * messages. It uses the asynchronous API of the UART driver, so the reception
 * is handled using interrupts and a callback function. The received data is stored
 * in a message queue for further processing.
 * 
 * NOTE: By default one uart is always configured to be used for the printk()
 * function. Just take a look at your board's .dts file to see which uart is
 * used for that purpose and avoid using it in this sample.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

/* define a structure to control data to transmit */
struct uart_rx{
    struct k_msgq queue;
    char queue_buf[50 * sizeof(uint8_t)];
    uint8_t dma_buf[2][50];
    uint8_t buf_index;
};

/* Get the device tree name for the uart port using an alias, 
as it is defined in the device tree file*/
const struct device *uart_dev = DEVICE_DT_GET( DT_ALIAS( uart ) );

/* This is the function we are going to call durint the uart ISR */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);

int main( void )
{
    struct uart_rx rx_buffer = {0};

    uint8_t buffer[50];
    uint8_t counter = 0;

    k_msgq_init(&rx_buffer.queue, rx_buffer.queue_buf, sizeof(uint8_t), 50);

    /* set the callback function to be used on interrupts */
    uart_callback_set( uart_dev, uart_cb, &rx_buffer );
	/*enable reception and set the buffer, las paramter indicates a timeout
	to enter the callback sinde the first character have been recieved, 10ms*/
    uart_rx_enable( uart_dev, (uint8_t*)rx_buffer.dma_buf, sizeof( rx_buffer.dma_buf[0] ), 10000 );

    while(1)
    {
        /* wait for a character to be received, chances are than more the one
        character has been received, in that case will be more efficient to
        know the number of characters received using the k_msgq_num_used_get */
        k_msgq_get( &rx_buffer.queue, &buffer[counter], K_FOREVER );
        /*process the character received*/
        if( buffer[counter] == '\r' ){
            buffer[counter] = '\0'; // null terminate the string
            counter = 0;
            printk( "mesage received: %s\n", buffer );
        }
        else{
            counter++;
            if( counter >= sizeof(buffer) ){
                /* prevent buffer overflow */
                counter = 0;
            }
        }
    }

    return 0;
}

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    struct uart_rx *rx_buff = user_data;
	
	switch (evt->type) {
		case UART_RX_BUF_REQUEST:
			/* to be eable to receive a message again we need to refresh the reception buffer
			to avoid information overruns we switch between at least two buffers */
			uart_rx_buf_rsp( dev, &rx_buff->dma_buf[rx_buff->buf_index][0], sizeof(rx_buff->dma_buf[0]) );
            rx_buff->buf_index ^= 1;
		break;
        case UART_RX_RDY:
            /*because we are using a queue to store the received data, we can ignore the fact 
            we can get here for a buffer partialy filled or completely filled, we just need to
            process the data received simply by going trough the data received and store each 
            byte into a queue, just make sure the queue has sufficient space, if not the queue
            internal ring buffer will overwrite old data */
			for( uint8_t i = evt->data.rx.offset ; i < evt->data.rx.len ; i++ ){
                /*validate is a valid ASCII character*/
                if( ( evt->data.rx.buf[ i ] >= ' ' && evt->data.rx.buf[ i ] <= '~' ) || evt->data.rx.buf[ i ] == '\r' ){
                    /* put the received byte into the message queue */
                    k_msgq_put( &rx_buff->queue, &evt->data.rx.buf[ i ], K_NO_WAIT );
                }
			}
        break;
        default:
        break;
    }
}