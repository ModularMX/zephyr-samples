/**
 * @file main.c
 * @brief Sending and receiving mesages with a lifo
 * 
 * LIFO are basically queues that only prepend messages, actually its API are 
 * pretty much defines from queue API plus some trace macros, there is no 
 * function to append because LIFO only prepend messages.
 */
#include <zephyr/kernel.h>

/*define a data structure use as lifo element, the first
element will always use by the lifo to point the next element*/
struct lifo_node {
    sys_snode_t snode;  /*next element address*/
    uint32_t data;      /*lifo payload*/
};

/* Define the fucntions used as threads */
void task_sender( void *p1, void *p2, void *p3 );
void task_receiver( void *p1, void *p2, void *p3 );

/*define a new lifo*/
K_LIFO_DEFINE(my_lifo);

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(sender_id, 1024, task_sender, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 5, 0, 0);

/* Define the function used as thread */
void task_sender( void *p1, void *p2, void *p3 )
{
    /*we declare an array of nodes to be use as memory pool for our lifo
    it is not necesary to have all lifo nodes in contiguous memory locations
    but it is easier to manage this way*/
    struct lifo_node mem_pool[5] = {0};
    
	while(1) 
	{
        printk("Filling the lifo with data\r");
        /*append 5 messages to the lifo, there is limit in the number of messages
        we can placed, but in our case we are limited by the size of the memory pool*/
        for( int i=0 ; i<5 ; i++ ){
            mem_pool[i].data = i * 10;
            k_lifo_put( &my_lifo, &mem_pool[i] );
        }
        /* Send thread to sleep for 1000ms */
		k_msleep( 1000 );
	}
}

/* Define the function used as thread */
void task_receiver( void *p1, void *p2, void *p3 )
{
    struct lifo_node *rx_data;
    
	while(1) 
	{
        /*send thread to sleep for 1000ms */
        k_msleep( 1000 );

        /*read all messages available in the lifo, at this point the lifo should be "full"
        NOTE: ther is no k_lifo_is_empty() function, so we use k_queue_is_empty() instead*/
        while(k_queue_is_empty(&(my_lifo._queue)) == 0){
            /*we do not need to wait because we already know the fifo is not empty.
            the function only retrieve the address of oldest available element
            in the fifo from the memory pool at the sender thread, it is application
            reponsability to make a copy if needed*/
            rx_data = k_lifo_get(&my_lifo, K_NO_WAIT);
            printk( "Data received from the lifo: %d\r", rx_data->data );
        }
	}
}
