/**
 * @file main.c
 * @brief Two Timers with same Callback Sample
 * 
 * This sample demonstrates the use of a same callback function for two different
 * software timers. Each timer is started with a different period. The callback
 * function identifies which timer expired using the timer ID passed as a parameter.
 */
#include <zephyr/kernel.h>

/* Define the functions used when the timer expired */
void expiry_callback( struct k_timer *timer_id );

/* declare two software timers called timer_id1 and timer_id2, 
sharing the same expiry_callback with no stop callback function */
K_TIMER_DEFINE( timer_id1, expiry_callback, NULL );
K_TIMER_DEFINE( timer_id2, expiry_callback, NULL );

int main( void )
{
    /* start two timers with different periods */
    k_timer_start( &timer_id1, K_MSEC(1000), K_MSEC(1000) );
    k_timer_start( &timer_id2, K_MSEC(2000), K_MSEC(2000) );
}

void expiry_callback( struct k_timer *timer_id )
{
    /*using the timer id we can identify which timer expired */
    if( timer_id == &timer_id1 ) {
        static int count1 = 0;
        count1++;
        printk("Timer 1 expired %d times\n", count1 );
    } 
    else if( timer_id == &timer_id2 ) {
        static int count2 = 0;
        count2++;
        printk("Timer 2 expired %d times\n", count2 );
    }
}