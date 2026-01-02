/**
 * @file main.c
 * @brief Sending and receiving more than one message using a queue
 * 
 * This sample application shows how to send and receive multiple messages
 * using a Zephyr kernel queue. A sender thread fills the queue with multiple
 * messages, while a receiver thread retrieves and processes these messages.
 * fucntion k_queue_is_empty is used to retrieve all of them untill the queue is empty.
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
K_THREAD_DEFINE(sender_id, 1024, task_sender, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 5, 0, 0);

/* Define the function used as thread */
void task_sender( void *p1, void *p2, void *p3 )
{
    /*we declare an array of nodes to be use as memory pool for our queue
    it is not necesary to have all queue nodes in contiguous memory locations
    but it is easier to manage this way*/
    struct queue_node mem_pool[5] = {0};
    
	while(1) 
	{
        printk("Filling the queue with data\r");
        /*append 5 messages to the queue, there is limit in the number of messages
        we can placed, but in our case we are limited by the size of the memory pool*/
        for( int i=0 ; i<5 ; i++ ){
            mem_pool[i].data = i * 10;
            k_queue_append( &my_queue, &mem_pool[i] );
        }
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
        /*send thread to sleep for 1000ms */
        k_msleep( 1000 );

        /*read all messages available in the queue, at this point the queue should be "full"*/
        while(k_queue_is_empty(&my_queue) == 0){
            /*we do not need to wait because we already know the queue is not empty.
            the function only retrieve the address of oldest available element
            in the queue from the memory pool at the sender thread, it is application
            reponsability to make a copy if needed*/
            rx_data = k_queue_get(&my_queue, K_NO_WAIT);
            printk( "Data received from the queue: %d\r", rx_data->data );
        }
	}
}
