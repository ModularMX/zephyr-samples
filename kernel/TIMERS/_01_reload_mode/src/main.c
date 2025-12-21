/**
 * @file main.c
 * @brief TIMERS Reload Mode Sample
 * 
 * This sample demonstrates the use of a software timer in reload mode. A timer
 * is created that expires for the first time after 2 seconds and then
 * continues to expire every 500 milliseconds. Each time the timer expires,
 * a callback function is invoked that increments and prints a counter.
 */
#include <zephyr/kernel.h>

/* Define the functions used when the timer expired */
void expiry_callback( struct k_timer *timer_id );

/* declare a software timer called timer_id, and expiry_callback
with no stop callback function */
K_TIMER_DEFINE( timer_id, expiry_callback, NULL );

int main( void )
{

    /* start a periodic timer that expires at 2 seconds for the first time
    then continues expiring with a period of 500ms */
    k_timer_start( &timer_id, K_MSEC(2000), K_MSEC(500) );
}

void expiry_callback( struct k_timer *timer_id )
{
    static int count = 0;
    printk("Timer expired %d times\n", count++);
}