/**
 * @file main.c
 * @brief Reading all messages from a message queue sample
 * 
 * This sample application demonstrates how to read all messages from a message queue. 
 * One thread fills the queue with incrementing integer values every second,
 * while another thread reads and processes all messages available in the queue at once.
 * function k_msgq_num_used_get() is used to check how many messages are currently 
 * in the queue.
 */
#include <zephyr/kernel.h>

/* Define the functions used as threads */
void task_sender( void *p1, void *p2, void *p3 );
void task_receiver( void *p1, void *p2, void *p3 );

/* queue definition in build time, 10 objects of size long */
K_MSGQ_DEFINE(my_msgq, sizeof(uint32_t), 10, 1);

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(sender_id, 1024, task_sender, NULL, NULL, NULL, 6, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 7, 0, 0);

/* Define the function used as thread */
void task_sender( void *p1, void *p2, void *p3 )
{
	uint32_t data = 0;

	while(1) 
	{
        /*fill the queue until it is full */
        while( k_msgq_num_free_get( &my_msgq ) > 0 ){
            data++;
            /* put a new message into the queue */
		    k_msgq_put( &my_msgq, &data, K_NO_WAIT );
        }
        /* Send thread to sleep for 1000ms */
		k_msleep( 1000 );
	}
}

/* Define the function used as thread */
void task_receiver( void *p1, void *p2, void *p3 )
{
    uint32_t data;

	while(1) 
	{
        k_msleep(1000);
        /*read all messges in the queue until none of them left*/
        while( k_msgq_num_used_get( &my_msgq ) != 0 ){
            /* read a message from the queue if empty suspend the task
            until a new message arrives */
		    k_msgq_get( &my_msgq, &data, K_NO_WAIT );
            /* just print the new message received*/
            printk( "Received Value = %d\n", data );
        }
	}
}