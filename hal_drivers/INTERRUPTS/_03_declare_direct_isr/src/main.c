/**
 * @file main.c
 * @brief Set up a custom direct interrupt service routine (ISR)
 * 
 * This sample code demonstrates how to set up a custom direct interrupt service routine (ISR)
 * The macro IRQ_DIRECT_CONNECT make the ISR is not place in the _sw_isr_table, this avboid 
 * to be called by the wrapper funtion, intead is place directly in the _irq_vector_table
 * as you usually expect in a Cortex-M arquitecture, but the ISR is not called directly, intead of.
 * is been called throu a compiled defined custom macro which include a cpuple of Zephyr macros
 * to evalute if the kernel needs to make a desition after the ISR
 */
#include <zephyr/kernel.h>

/*
* - on stm32g0 devices this is the Power voltage detector interrupt
* - on stm32h5 devices this is the Power voltage monitor interrupt
* - on nrf54l15 devices this is the RADIO interrupt
* - on nxpn9450 devices this is the eDMA_0_CH0 error or transfer complete interrupt
* - on mcxa156 devices this is the CMC (Core Mode Controller) interrupt
0x80A3B7D */
#define DUMMY_ISR      1 

/*we need to declare our ISR using the following macro, this macro will declare
and inline fucntion called my_own_isr and call another one called my_own_isr_body
which in turn will be surounbded by the macros ISR_DIRECT_HEADER and ISR_DIRECT_FOOTER
used by the kernel to decided if a desition should be made */
ISR_DIRECT_DECLARE(my_own_isr)
{
    printk( "Interrupt routine\n\r" );
    
    /* PM done after servicing interrupt for best latency. It performs tasks necessary to
    exit power management idle state*/
    ISR_DIRECT_PM();
    
    /* Ask the kernel to check if scheduling decision should be made when return 1 */
    return 1;
}

int main( void )
{
    /*set up inline function my_own_isr to IRQ vector 1 directly on _irq_vector_table 
    (file isr_table.c) at index 1, so the NVIC can called as soon as the ISR gets 
    triggered, this direct ISR doesn't accept parameters*/
    IRQ_DIRECT_CONNECT( DUMMY_ISR, 1, my_own_isr, 0 );
    
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
