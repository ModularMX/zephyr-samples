/**
 * @file main.c
 * @brief TIMERS One Shot Mode Sample
 * 
 * This sample demonstrates the use of a software timer in one shot mode. 
 * A timer is created that expires after 2 seconds, and upon expiration,
 * it prints a message and restarts itself to expire every 1 second thereafter.
 * 
 * NOTE: the timer callback function runs in interrupt context, so it should
 * execute quickly and not perform blocking operations.
 */
#include <zephyr/kernel.h>

/* Define the functions used when the timer expired */
void expiry_callback( struct k_timer *timer_id );

/* declare a software timer called timer_id, and expiry_callback
with no stop callback function */
K_TIMER_DEFINE( timer_id, expiry_callback, NULL );

int main( void )
{

    /* start a periodic timer that expires at 2 seconds, 
    then do nothing ... */
    k_timer_start( &timer_id, K_MSEC(2000), K_NO_WAIT );
}

void expiry_callback( struct k_timer *timer_id )
{
    static int count = 0;
    printk("Timer expired %d times\n", count++);
    /*lets do it again, just five times more*/
    if(count >= 5)
    {
        printk("Timer demo ended\n");
        return;
    }
    k_timer_start( timer_id, K_MSEC(1000), K_NO_WAIT );
}