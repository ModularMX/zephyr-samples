/**
 * @file main.c
 * @brief Single ISR-thread communication
 * 
 * This sample application demonstrates communication between an ISR and a thread
 * using a message queue. The ISR sends incrementing integer values to the queue,
 * and the receiver thread processes these messages.
 * It is very important to always use k_NO_WAIT when using message queue APIs
 * in an ISR context.
 * 
 * NOTE: sample only tested on Cortex-M architectures.
 */
#include <zephyr/kernel.h>

#define DUMMY_ISR  15  /* Dummy IRQ number for demonstration */

/* Define the fucntions used as threads */
void task_receiver( void *p1, void *p2, void *p3 );
/* Define the function used as ISR */
void my_own_isr( void *p );

/* queue definition in build time, 10 objects of size long */
K_MSGQ_DEFINE(my_msgq, sizeof(uint32_t), 10, 1);

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 6, 0, 0);

int main( void )
{
    /*set up function my_own_isr to IRQ vector, no paramters, no flags */
    IRQ_CONNECT( DUMMY_ISR, 1, my_own_isr, NULL, 0 );
    /*enable interrupt*/
    irq_enable( DUMMY_ISR );

    while(1)
    {
        /*NOTE: at the moment Zephyr does not have an interface 
        to trigger interrupts manually, this is why we are using the
        CMCIS NVIC API*/
        NVIC_SetPendingIRQ( DUMMY_ISR );

        k_msleep( 1000 );
    }
    
    return 0;
}

void my_own_isr( void *p )
{
    static uint32_t data = 0;

    /* put a new message into the queue, mandatory to use always K_NO_WAIT
    on any message queue API use in ISR context */
	data++;
    k_msgq_put( &my_msgq, &data, K_NO_WAIT );
}

/* Define the function used as thread */
void task_receiver( void *p1, void *p2, void *p3 )
{
    uint32_t data;

	while(1) 
	{
        /* read a message from the queue if empty suspend the task
        until a new message arrives */
		k_msgq_get( &my_msgq, &data, K_FOREVER );
        /* just print the new message received*/
        printk( "Received Value = %d\n", data );
	}
}