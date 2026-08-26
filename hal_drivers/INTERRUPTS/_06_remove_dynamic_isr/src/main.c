/**
 * @file main.c
 * @brief Disconnect a dynamic ISR from an interrupt line
 * 
 * This sample code demonstrates how to disconnect a dynamic interrupt service routine (ISR) 
 * from an interrupt line in Zephyr. It sets up two ISRs on the same interrupt vector and 
 * then disconnects one of them after a certain period of time, allowing only the remaining 
 * ISR to be called when the interrupt is triggered. After another period of time, it 
 * reconnects the second ISR, allowing both ISRs to be called again when the interrupt is 
 * triggered.
 * 
 * It is not allowed to disconnect all interrupts from the interrupt line, in case you don't want
 * any ISR to be called it ios better to disable the interrupt 
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

void my_first_isr( const void *p );
void my_second_isr( const void *p );

int main( void )
{
    int counter = 0;
    /*set two ISR on the same IRQ vector on run time, yes this is basically
    a shared interrupt line*/
    irq_connect_dynamic( DUMMY_ISR, 1, my_first_isr, NULL, 0 );
    irq_connect_dynamic( DUMMY_ISR, 1, my_second_isr, NULL, 0 );
    /*enable interrupt*/
    irq_enable( DUMMY_ISR );

    while(1)
    {
        /*NOTE: at the moment Zephyr does not have an interface to trigger 
        interrupts manually, this is why we are using the CMSIS NVIC API*/
        NVIC_SetPendingIRQ( DUMMY_ISR );

        k_msleep( 1000 );
        counter++;

        /*after 5 seconds disconnect my_second_isr from the interrupt line, 
        this will cause only my_first_isr to be called when the interrupt is 
        triggered*/
        if( counter == 5 ){
            printk( "Disconnecting my_second_isr interrupt\n\r" );
            irq_disconnect_dynamic( DUMMY_ISR, 1, my_second_isr, NULL, 0 );
        }

        /*after 10 seconds reconnect my_second_isr to the interrupt line,
        this will cause both my_first_isr and my_second_isr to be called 
        when the interrupt is triggered*/
        if( counter == 10 ){
            printk( "Reconnecting my_second_isr interrupt\n\r" );
            irq_connect_dynamic( DUMMY_ISR, 1, my_second_isr, NULL, 0 );
            counter = 0;
        }
    }
    
    return 0;
}

/*this is the function that will be called when the interrupt occurs 
you must somehow determine if this is the subroutine that shall run
when the interrupt is triggered*/
void my_first_isr( const void *p )
{
    printk( "First interrupt routine\n\r" );
}

/*this is the function that will be called when the interrupt occurs 
you must somehow determine if this is the subroutine that shall run
when the interrupt is triggered*/
void my_second_isr( const void *p )
{
    printk( "Second interrupt routine\n\r" );
}