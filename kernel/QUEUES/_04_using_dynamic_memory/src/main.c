/**
 * @file main.c
 * @brief Uusing dynamic memory with kernel queues
 * 
 * The example shows how to use dynamic memory allocation to create queue nodes 
 * for a kernel queue. A sender thread fills the queue with data until the heap
 * is full, then a receiver thread reads all the data from the queue and frees 
 * the memory used by each node.
 * 
 * NOTE: alloc memory implies a certain overhead so the number of nodes that can be
 * created is less than the pure data size divided by the node size.
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

#define HEAP_DATA   52
#define NODE_DATA  (sizeof(struct queue_node)+8) /*8 bytes of sys_snode_t overhead*/

/*define a heap able to store at least 5 queue nodes.
At the moment i'm not sure about how to calculate the amount of memory needed
seems like the caculation obey the following formula more or less
heap_ctr_data + ((node_data + data_padding) * number_of_nodes)*/
K_HEAP_DEFINE(queue_heap, HEAP_DATA + (NODE_DATA * 5)); 

/*define a new queue*/
K_QUEUE_DEFINE(my_queue);

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(sender_id, 1024, task_sender, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 5, 0, 0);

/* Define the function used as thread */
void task_sender( void *p1, void *p2, void *p3 )
{
    struct queue_node *ptr_node;
    uint32_t count = 0;

	while(1) 
	{
        printk("Sending data to the queue...\r");
        /*We reserve enough memory from the heap to create at least 5 fives queue nodes
        we do not need to count them, we can reserve memory until the heap is full*/
        while( (ptr_node = k_heap_alloc(&queue_heap, sizeof(struct queue_node), K_NO_WAIT)) != NULL ) {
            ptr_node->data = count++; /*fill the node with some data*/
            k_queue_append( &my_queue, ptr_node );
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
            rx_data = k_queue_get( &my_queue, K_NO_WAIT );
            printk( "Data received from the queue: %d\r", rx_data->data );
            /*free the memory allocated for the node, k_queue_get remove the element 
            from the queue, but it doesn't free the memory each node occupy in the 
            heap, no risk for memory fragmentation becasue we are always using the same size*/
            k_heap_free(&queue_heap, rx_data);
        }
	}
}
