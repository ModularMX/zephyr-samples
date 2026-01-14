/**
 * @file main.c
 * @brief Single queue create in runtime
 * 
 * This sample application demonstrates the use of a single message queue
 * for inter-thread communication it ismilar as previous example, but this time.
 * the queue is created at runtime instead of build time using the k_msgq_init() function.
 * main funtion runs in the highest priority context, so it creates the queue
 * before the two threads starts to run.
 */
#include <zephyr/kernel.h>

/* Define the fucntions used as threads */
void task_sender( void *p1, void *p2, void *p3 );
void task_receiver( void *p1, void *p2, void *p3 );

/* declare queue to communicate both tasks */
struct k_msgq my_msgq;
/* queue buffer to store data */
uint8_t my_msgq_buffer[ 10 * sizeof(uint32_t) ];

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(sender_id, 1024, task_sender, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 6, 0, 0);

int main( void )
{
    /* create a queue to communicate among the three tasks */
    k_msgq_init(&my_msgq, my_msgq_buffer, sizeof(uint32_t), 10);

    return 0;
}

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