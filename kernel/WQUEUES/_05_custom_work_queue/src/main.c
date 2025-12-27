/**
 * @file main.c
 * @brief Submitting a single work to user define work queue thread
 * 
 * The sample shows how to define a specific thread to run "works", this new thread
 * will be defined as a work queue and it will execute its own "works" avoiding the
 * use of the system work queue. it has its own priority and stack
 */
/* Include libraries */
#include <zephyr/kernel.h>

/*define a stack with 1024 bytes for our own work queue*/
K_THREAD_STACK_DEFINE( wq_stack, 1024 );
/* declare a work queue thread control structure */
struct k_work_q work_queue;

/*This is the function acting as the "work to do"*/
void work_handler( struct k_work *work );

/*define a new work to do, "work_to_queue" is the handler 
for this work and "work_handler is the function to be called"*/
K_WORK_DEFINE( work_to_queue, work_handler );

int main( void )
{
    /*just zeroed the k_work_q variable*/
    k_work_queue_init( &work_queue );
    /*create a new work queue, with a priority of five and no extra flags*/
    k_work_queue_start( &work_queue, wq_stack, K_THREAD_STACK_SIZEOF( wq_stack ), 5, NULL );

    while(1)
    {
        /* send something to do on our user defined work queue*/
        k_work_submit_to_queue( &work_queue, &work_to_queue );

        k_msleep( 1000 );
    }
}

/*the function will be called "in" the user work queue thread
active by default when a "work" has been submitted*/
void work_handler( struct k_work *work ) 
{
    printk("Work to do on user workqueue\r");
}