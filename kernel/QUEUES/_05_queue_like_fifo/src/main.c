/**
 * @file main.c
 * @brief Sending and receiving mesages with a fifo
 * 
 * FIFO as basically queues that only append messages, actually its API are 
 * pretty much defines from queue API plus some trace macros, there is no 
 * fucntion to preppend because FIFO only append messages at the end.
 */
#include <zephyr/kernel.h>

/*define a data structure use as fifo element, the first
element will always use by the fifo to point the next element*/
struct fifo_node {
    sys_snode_t snode;  /*next element address*/
    uint32_t data;      /*fifo payload*/
};

/* Define the fucntions used as threads */
void task_sender( void *p1, void *p2, void *p3 );
void task_receiver( void *p1, void *p2, void *p3 );

/*define a new fifo*/
K_FIFO_DEFINE(my_fifo);

/* Thread definition (build time, run immediately) */
K_THREAD_DEFINE(sender_id, 1024, task_sender, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(receiver_id, 1024, task_receiver, NULL, NULL, NULL, 5, 0, 0);

/* Define the function used as thread */
void task_sender( void *p1, void *p2, void *p3 )
{
    /*we declare an array of nodes to be use as memory pool for our fifo
    it is not necesary to have all fifo nodes in contiguous memory locations
    but it is easier to manage this way*/
    struct fifo_node mem_pool[5] = {0};
    
	while(1) 
	{
        printk("Filling the fifo with data\r");
        /*append 5 messages to the fifo, there is limit in the number of messages
        we can placed, but in our case we are limited by the size of the memory pool*/
        for( int i=0 ; i<5 ; i++ ){
            mem_pool[i].data = i * 10;
            k_fifo_put( &my_fifo, &mem_pool[i] );
        }
        /* Send thread to sleep for 1000ms */
		k_msleep( 1000 );
	}
}

/* Define the function used as thread */
void task_receiver( void *p1, void *p2, void *p3 )
{
    struct fifo_node *rx_data;
    
	while(1) 
	{
        /*send thread to sleep for 1000ms */
        k_msleep( 1000 );

        /*read all messages available in the fifo, at this point the fifo should be "full"*/
        while(k_fifo_is_empty(&my_fifo) == 0){
            /*we do not need to wait because we already know the fifo is not empty.
            the function only retrieve the address of oldest available element
            in the fifo from the memory pool at the sender thread, it is application
            reponsability to make a copy if needed*/
            rx_data = k_fifo_get(&my_fifo, K_NO_WAIT);
            printk( "Data received from the fifo: %d\r", rx_data->data );
        }
	}
}
