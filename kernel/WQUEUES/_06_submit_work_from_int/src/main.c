/**
 * @file main.c
 * @brief Submitting a single work from an ISR
 * 
 * This is one the main purpose of the zephyr work queues to offload an ISR rutine.
 * rather than using an ISR attend the event trigger the interrupt we can chose to execute
 * that code in a separate thread, we just need to considered there is latency inherited 
 * by the same work queue mechanism and context switch 
 */
/* Include libraries */
#include <zephyr/kernel.h>

#define DUMMY_ISR      1 /*on stm32g0 devices this is the Power voltage detector interrupt*/

/*function used as interrupt vector*/
void my_own_isr( void *p );

/*This is the function acting as the "work to do"*/
void work_handler( struct k_work *work );

/*define a new work to do, "work_to_queue" is the handler 
for this work and "work_handler is the function to be called"*/
K_WORK_DEFINE( work_to_queue, work_handler );

int main( void )
{
    /*set up function my_own_isr to IRQ vector, no paramters, no flags */
    IRQ_CONNECT( DUMMY_ISR, 1, my_own_isr, 100, 0 );
    /*enable interrupt*/
    irq_enable( DUMMY_ISR );

    while(1)
    {
        /*NOTE: at the moment Zephyr does not have an interface to trigger 
        interrupts manually, this is why we are using the CMCIS NVIC API*/
        NVIC_SetPendingIRQ( DUMMY_ISR );

        k_msleep( 1000 );
    }
}

void my_own_isr( void *p )
{
    /* send something to do*/
    k_work_submit( &work_to_queue );
}

/*the function will be called "in" the system work queue thread
active by default when a "work" has been submitted*/
void work_handler( struct k_work *work ) 
{    
    printk("Work to do on system workqueue\r");
}