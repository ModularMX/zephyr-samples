/**
 * @file main.c
 * @brief Pass data to an interrupt service routine (ISR)
 * 
 * This sample code demonstrates how to set up a custom interrupt service routine (ISR)
 * The macro IRQ_CONNECT set the the ISR number 17 or 1 and link the function my_own_isr to it, 
 * and also pass the address of the data variable as a parameter to the ISR.
 * You can pass any data you like to the ISR, but in this example we are passing a simple 
 * uint8_t variable with the value of 100.
 */
#include <zephyr/kernel.h>

/*
* - on stm32g0 devices this is the Power voltage detector interrupt
* - on stm32h5 devices this is the Power voltage monitor interrupt
* - on nrf54l15 devices this is the RADIO interrupt
* - on nxpn9450 devices this is the eDMA_0_CH0 error or transfer complete interrupt
* - on mcxa156 devices this is the CMC (Core Mode Controller) interrupt
*/
#define DUMMY_ISR      1 

void my_own_isr( void *p );
const uint8_t data = 100;

int main( void )
{
    /*set up function my_own_isr to IRQ vector 1, parameter, no flags.
    this basically set the my_own_isr function address and the data variable 
    address in the array _sw_isr_table (file isr_table.c) at index 1, this 
    array is used by the _isr_wrapper function to call the correct ISR when 
    an interrupt occurs*/
    IRQ_CONNECT( DUMMY_ISR, 1, my_own_isr, &data, 0 );
    
    /*enable interrupt*/
    irq_enable( DUMMY_ISR );

    while(1)
    {
        /*NOTE: at the moment Zephyr does not have an interface to trigger 
        interrupts manually, this is why we are using the CMSIS NVIC API*/
        NVIC_SetPendingIRQ( DUMMY_ISR );

        k_msleep( 1000 );
    }
    
    return 0;
}

/*this is the function that will be called when the interrupt occurs 
is not a good idea to call a function like printk inside an ISR*/
void my_own_isr( void *p )
{
    uint8_t data = *(uint8_t *)p; 
    printk( "Interrupt data :%d\n\r", data );
}