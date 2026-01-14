/**
 * @file main.c
 * @brief Many to one thread communication via message queue sample
 * 
 * This sample application demonstrates the use of a single message queue
 * for more than two thread communication. Multiple sender threads send structured 
 * data to a single receiver thread via a message queue. Receiver has the highest
 * priority to ensure that messages are processed as soon as they arrive.
 * One function is defined for the sender threads and thread parameters are used to 
 * differentiate them.
 */
#include <zephyr/kernel.h>

struct data {
    uint8_t value;    /* Variable to store the message of the sender */
    uint8_t source;   /* Variable to identify the Sender */
};

/* Define the fucntions used as threads */
void task_sender( void *p1, void *p2, void *p3 );
void task_receiver( void *p1, void *p2, void *p3 );

/* queue definition in build time, 10 objects of size data */
K_MSGQ_DEFINE(my_msgq, sizeof(struct data), 10, 1);

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(sender1_id, 1024, task_sender, 101, 1, NULL, 7, 0, 0);
K_THREAD_DEFINE(sender2_id, 1024, task_sender, 202, 2, NULL, 7, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 5, 0, 0);

/* Define the function used as thread */
void task_sender( void *p1, void *p2, void *p3 )
{
    struct data data_to_send;
    
	while(1) 
	{
        /*sets values to send*/
        data_to_send.value = (uint32_t)p1;
        data_to_send.source = (uint32_t)p2; 
        /* put a new message into the queue */
		k_msgq_put( &my_msgq, &data_to_send, K_NO_WAIT );
		/* Send thread to sleep for 500ms */
		k_msleep( 500 );
	}
}

/* Define the function used as thread */
void task_receiver( void *p1, void *p2, void *p3 )
{
    struct data data;

	while(1) 
	{
        /* read a message from the queue if empty suspend the task
        until a new message arrives */
		k_msgq_get( &my_msgq, &data, K_FOREVER );
        /* just print the new message received*/
        printk( "Received Value = %d from: %d\n", data.value, data.source );
	}
}