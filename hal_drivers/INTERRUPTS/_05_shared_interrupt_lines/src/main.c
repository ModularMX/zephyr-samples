/**
 * @file main.c
 * @brief Set up two shared interrupts in one interrupt line
 * 
 * This sample code demonstrates how to set up two shared interrupts in one interrupt line. 
 * The two ISRs will be called in order of registration. A new table z_shared_sw_isr_table
 * will be created to keep track of the clients sharing the same interrupt line. The shared
 * interrupt line will be enabled and triggered in the main loop. The two ISRs will print
 * a message when they are called.
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

void my_first_isr( void *p );
void my_second_isr( void *p );

int main( void )
{
    /*set up functions my_first_isr and my_second_isr to IRQ vector 1. None of the two 
    ISR address will be populated in _sw_isr_table but a new wrapper exclusive for both
    ISRs will be created and this function is the one calling both ISRs in order of
    registration.*/
    IRQ_CONNECT( DUMMY_ISR, 1, my_first_isr, NULL, 0 );
    IRQ_CONNECT( DUMMY_ISR, 1, my_second_isr, NULL, 0 );
    
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
you must somehow determine if this is the subroutine that shall run
when the interrupt is triggered*/
void my_first_isr( void *p )
{
    printk( "First shared interrupt routine\n\r" );
}

/*this is the function that will be called when the interrupt occurs 
you must somehow determine if this is the subroutine that shall run
when the interrupt is triggered*/
void my_second_isr( void *p )
{
    printk( "Second shared interrupt routine\n\r" );
}