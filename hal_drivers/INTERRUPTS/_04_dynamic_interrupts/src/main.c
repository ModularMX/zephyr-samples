/**
 * @file main.c
 * @brief Set up a custom dynamic interrupt
 * 
 * This sample code demonstrates how to set up a custom dynamic interrupt service routine (ISR)
 * It is bnasically the same as the static interrupt sample, but instead of using the 
 * IRQ_CONNECT() macro, it uses the irq_connect_dynamic() function to set up the interrupt 
 * at runtime. The array at _sw_isr_table won't have any address at index 1 until the 
 * function irq_connect_dynamic() is called, after that the address of my_own_isr will be stored.
 * 
 * It is mandatory to set CONFIG_DYNAMIC_INTERRUPTS
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

void my_own_isr( const void *p );

int main( void )
{
    /*set index 1 (IRQ vector 1) _sw_isr_table with address function my_own_isr to 
    , no paramters, no flags. This array is used by the _isr_wrapper 
    function to call the correct ISR when an interrupt occurs*/
    irq_connect_dynamic( DUMMY_ISR, 1, my_own_isr, NULL, 0 );
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
void my_own_isr( const void *p )
{
    printk( "Dynamic Interrupt routine\n\r" );
}