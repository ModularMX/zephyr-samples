/**
 * @file main.c
 * @brief Lock and unlock interrupts sample
 * 
 * This sample code demonstrates how to lock and unlock interrupts using the Zephyr API. 
 * It sets up a dummy interrupt service routine (ISR) that increments a counter each time 
 * it is called. The main loop triggers the interrupt every second, and after five triggers, 
 * it locks interrupts, triggers the interrupt again, and then unlocks interrupts 
 * to allow the ISR to be executed.
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
    unsigned int lock;
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
        /*disable interrupts with a priority above 0*/
        lock = irq_lock();
        /*after the next line the interrupt triggered by the previous NVIC_SetPendingIRQ 
        call will not be handled until the irq_unlock function is called*/
        NVIC_SetPendingIRQ( DUMMY_ISR );
        printk( "Interrupts locked\n\r" );
        /*after the next line the interrupt triggered by the previous NVIC_SetPendingIRQ 
        call will be handled*/
        irq_unlock( lock );
    }
    
    return 0;
}

/*this is the function that will be called when the interrupt occurs 
is not a good idea to call a function like printk inside an ISR*/
void my_own_isr( void *p )
{
    volatile static int count = 0;

    count++;
    printk( "Interrupt routine, count = %d\n\r", count );
}