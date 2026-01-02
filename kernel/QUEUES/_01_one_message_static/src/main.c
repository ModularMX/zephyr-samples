/**
 * @file main.c
 * @brief Application demonstrating static queue usage.
 * 
 * This sample application shows how to create and use a queue using uisng static
 * allocated memory for queue elements, it is important to notice the queue only pass
 * reference to one thread to another not the actual data, this remains stored in 
 * the sender thread. the advantege of using queues is that uses Zephyr kernel mechanism
 * to suspend the receiver thread when no data is available in the queue, and wake it up
 * when new data arrives.
 */
#include <zephyr/kernel.h>

/*define a data structure use as queue element, the first
element will always use by the queue to point the next element*/
struct queue_node {
    sys_snode_t snode;  /*next element address*/
    uint32_t data;      /*queue payload*/
};

/* Define the fucntions used as threads */
void task_sender( void *p1, void *p2, void *p3 );
void task_receiver( void *p1, void *p2, void *p3 );

/*define a new queue*/
K_QUEUE_DEFINE(my_queue);

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(sender_id, 1024, task_sender, NULL, NULL, NULL, 6, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 5, 0, 0);

/* Define the function used as thread */
void task_sender( void *p1, void *p2, void *p3 )
{
    /*the sender thread shall always be in charge of reserve memory
    for the queue, you can do it a static way taken memory from 
    the thread itself*/
    struct queue_node mem_pool[2] = {0};
    
	while(1) 
	{
        printk("appended a message to a queue\r");
        mem_pool[0].data++;
        /*once the message is placed in the queue, the higher priority task
        will force a context change*/
        k_queue_append(&my_queue, &mem_pool[0]);
		
        /* Send thread to sleep for 1000ms */
		k_msleep( 1000 );
	}
}

/* Define the function used as thread */
void task_receiver( void *p1, void *p2, void *p3 )
{
    struct queue_node *rx_data;
    
	while(1) 
	{
        /*the receiver thread has a higher priority so it will get here first and then 
        wait until a new data is available in the queue, once data is available, it will
        force a context change*/
        rx_data = k_queue_get(&my_queue, K_FOREVER);
        printk( "Data received from the queue: %d\r", rx_data->data );
	}
}
