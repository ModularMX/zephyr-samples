/**
 * @file main.c
 * @brief Change UART runtime configuration sample
 * 
 * This sample application shows how to change the UART runtime configuration 
 * using the UART HAL driver API. the uart_config structure is filled with the 
 * desired configuration and then applied using the uart_configure() function.
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

int main( void )
{
    /* Configuration structure */
    struct uart_config uart_cfg;
    
    uart_cfg.baudrate  = 115200;
    uart_cfg.data_bits = UART_CFG_DATA_BITS_8;
    uart_cfg.stop_bits = UART_CFG_STOP_BITS_1;
    uart_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
    uart_cfg.parity    = UART_CFG_PARITY_NONE;    
    /* Set UART configuration */
    uart_configure( uart_dev, &uart_cfg );

    while(1)
    {
        /* send just one single character and wait until is sended */
        uart_poll_out( uart_dev, 'Y' );
        k_msleep( 1000 );
    }
    return 0;
}