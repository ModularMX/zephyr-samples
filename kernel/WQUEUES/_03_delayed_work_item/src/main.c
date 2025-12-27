/**
 * @file main.c
 * @brief Submitting a single work with a delay
 * 
 * The sample shows how to send a new "work" but with an implicity delay, meaning
 * we don't want the function acting as a work get executed as soon as possible by 
 * the system work queue, but rather after certain time has passed. To achive this 
 * we need to declare our work handler using the macro K_WORK_DELAYABLE_DEFINE and
 * then send a new work with k_work_schedule, where we specify the delay
 */
/* Include libraries */
#include <zephyr/kernel.h>

/*This is the function acting as the "work to do"*/
void work_handler( struct k_work *work );

/*define a new work to do, "work_to_queue" is the handler 
for this work and "work_handler is the function to be called"*/
K_WORK_DELAYABLE_DEFINE( work_to_queue, work_handler );

int main( void )
{
    while(1)
    {
        printk("About to send a new work\r");
        /* send some work to do, but with a delay of 10ms after
        is received by the system work queue*/
        k_work_schedule( &work_to_queue, K_MSEC(500) );
        
        k_msleep( 2000 );
    }
}

/*the function will be called "in" the system work queue thread
active by default when a "work" has been submitted*/
void work_handler( struct k_work *work ) 
{
    /*message will be displayed 500ms after the work was recieved 
    by the system work queue */
    printk("Work to do on system workqueue\r");
}