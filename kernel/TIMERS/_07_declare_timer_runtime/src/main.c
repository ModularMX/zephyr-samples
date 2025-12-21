/**
 * @file main.c
 * @brief Sample application for declaring and using a timer at runtime
 * 
 * This sample demonstrates how to declare, initialize, and start a timer
 * in run time rather than at build time. The timer is set to expire
 * after 2 seconds for the first time and then continues expiring every
 * 500 milliseconds.
 */
#include <zephyr/kernel.h>

/* Define the functions used when the timer expired */
void expiry_callback( struct k_timer *timer_id );

int main( void )
{
    /* declare a timer control structure  */
    static struct k_timer timer_id;

    /* init a new timer instance (this operation doesn't consume stack)*/
    k_timer_init( &timer_id, expiry_callback, NULL );
    /* start a periodic timer that expires at 2 seconds for the first time
    then continues expiring with a period of 500ms */
    k_timer_start( &timer_id, K_MSEC(2000), K_MSEC(500) );
}

void expiry_callback( struct k_timer *timer_id )
{
    static int count = 0;
    printk("Timer expired %d times\n", count++);
}