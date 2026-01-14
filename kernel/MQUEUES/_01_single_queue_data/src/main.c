/**
 * @file main.c
 * @brief Single queue thread communication
 * 
 * This sample application demonstrates the use of a single message queue
 * for inter-thread communication in Zephyr RTOS. One thread sends
 * incrementing integer values to a message queue every second, while
 * another thread get suspended while waiting until a new message arrives.
 */
#include <zephyr/kernel.h>

/* Define the functions used as threads */
void task_sender( void *p1, void *p2, void *p3 );
void task_receiver( void *p1, void *p2, void *p3 );

/* queue definition in build time, 5 objects of size long */
K_MSGQ_DEFINE(my_msgq, sizeof(uint32_t), 5, 1);

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(sender_id, 1024, task_sender, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 6, 0, 0);

/* Define the function used as thread */
void task_sender( void *p1, void *p2, void *p3 )
{
	uint32_t data = 0;

	while(1) 
	{
        data++;
        /* put a new message into the queue */
		k_msgq_put( &my_msgq, &data, K_NO_WAIT );
		/* Send thread to sleep for 1000ms */
		k_msleep( 1000 );
	}
}

/* This is the higher priority thread, it will run first but, it will find
the queue empty then blocks itself until a new message arrives */
void task_receiver( void *p1, void *p2, void *p3 )
{
    uint32_t data;

	while(1) 
	{
        /* read a message from the queue, if empty suspend the task
        until a new message arrives */
		k_msgq_get( &my_msgq, &data, K_FOREVER );
        /* just print the new message received*/
        printk( "Received Value = %d\n", data );
	}
}