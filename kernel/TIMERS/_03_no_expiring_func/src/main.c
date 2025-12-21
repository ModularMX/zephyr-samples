/**
 * @file main.c
 * @brief Software timer with no expiring function sample
 * 
 * This sample demonstrates the use of a software timer in Zephyr RTOS
 * that does not have an expiry function. The timer is started to expire
 * every second, and the main thread checks for timer expiration and
 * prints a message each time the timer expires.
 */
#include <zephyr/kernel.h>

/* declare a software timer called timer_id, and expiry_callback
with no stop callback function */
K_TIMER_DEFINE( timer_id, NULL, NULL );

int main( void )
{
    int count = 0;

    /* start a periodic timer that expires at 1 seconds then stops */
    k_timer_start( &timer_id, K_MSEC(1000), K_NO_WAIT );

    while(true)
    {
        /* check if the timer has expired */
        if(k_timer_remaining_get( &timer_id ) == 0)
        {
            count++;
            printk("Timer expired %d times\n", count);
            /* restart the timer */
            k_timer_start( &timer_id, K_MSEC(1000), K_NO_WAIT );
        }
        k_msleep(100);
    }
}
