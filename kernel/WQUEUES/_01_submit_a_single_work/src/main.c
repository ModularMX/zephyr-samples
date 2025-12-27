/**
 * @file main.c
 * @brief Submitting a single work to system work queue thread
 * 
 * The sample shows how to send a single "work" to a default system work queue, wichi in turn
 * is basically a thread where all "works" are actaully executed. Each work consiste in a 
 * function to be called with a user code ant the prototype void work_handler(struct k_work *);
 */
/* Include libraries */
#include <zephyr/kernel.h>

/*This is the function acting as the "work to do"*/
void work_handler( struct k_work *work );

/*define a new work to do, "work_to_queue" is the handler 
for this work and "work_handler is the function to be called"*/
K_WORK_DEFINE( work_to_queue, work_handler );

int main( void )
{
    while(1)
    {
        /* send something to do*/
        k_work_submit( &work_to_queue );

        k_msleep( 1000 );
    }
}

/*the function will be called "in" the system work queue thread
active by default when a "work" has been submitted*/
void work_handler( struct k_work *work ) 
{
    printk("Work to do on system workqueue\r");
}