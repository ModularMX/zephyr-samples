/**
 * @file main.c
 * @brief Set up a custom interrupt service routine (ISR)
 * 
 * This sample code demonstrates how to set up a custom interrupt service routine (ISR)
 * The macro IRQ_CONNECT set the the ISR number 17 or 1 if we are not counting the first 
 * 16 interrupts reserved by the cortex-M arquitecture. Even though the ARM MCU comes with 
 * NVIC to manage nested interrupts, Zephyr uses a common ISR called _isr_wrapper to manage 
 * all the interrupts, inside this wrapper the function my_own_isr is called when the interrupt 
 * number 1 is triggered.
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

int main( void )
{
    /*set up function my_own_isr to IRQ vector 1, no paramters, no flags.
    this basically set the my_own_isr address in the array _sw_isr_table 
    (file isr_table.c) at index 1, this array is used by the _isr_wrapper 
    function to call the correct ISR when an interrupt occurs*/
    IRQ_CONNECT( DUMMY_ISR, 1, my_own_isr, NULL, 0 );
    
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
    printk( "Interrupt routine\n\r" );
}