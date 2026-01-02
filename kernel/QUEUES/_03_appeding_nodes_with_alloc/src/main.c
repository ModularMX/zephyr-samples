/**
 * @file main.c
 * @brief Alloc node links and filling queue sample
 * 
 * The sample shows how to use the k_queue_alloc_append function to
 * fill a queue. The function will allocate the sys_snode_t needed to
 * link the elements in the queue from the heap, so the user only need
 * to provide the actual payload, no need to make a structure where
 * the first element shall be sys_snode_t.
 * NOTE: it is necesary to specify the CONFIG_HEAP_MEM_POOL_SIZE in .conf file
 */
#include <zephyr/kernel.h>

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
    /*we declare an array of memory to be use as memory pool for our queue*/
    uint32_t mem_pool[5] = {0};
    
	while(1) 
	{
        printk("Filling the queue with data\r");
        /*append 5 messages to the queue*/
        for( int i=0 ; i<5 ; i++ ){
            mem_pool[i] = i * 10;
            /*internally the function will reserve memory from the heap to append
            the sys_snode_t needed to link the elements in the queue*/
            k_queue_alloc_append( &my_queue, &mem_pool[i] );
        }
        /* Send thread to sleep for 1000ms */
		k_msleep( 1000 );
	}
}

/* Define the function used as thread */
void task_receiver( void *p1, void *p2, void *p3 )
{
    uint32_t *rx_data;
    
	while(1) 
	{
        /*send thread to sleep for 1000ms */
        k_msleep( 1000 );

        /*read all messages available in the queue, at this point the queue should be "full"*/
        while(k_queue_is_empty(&my_queue) == 0){
            /*The function will only retrieve the actual node payload, because
            the sys_snode_t used to link the elements in the queue was allocated
            internally by the k_queue_alloc_append function*/
            rx_data = k_queue_get(&my_queue, K_NO_WAIT);
            printk( "Data received from the queue: %d\r", *rx_data );
        }
	}
}