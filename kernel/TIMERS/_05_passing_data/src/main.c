/**
 * @file main.c
 * @brief Passing data to a timer callback sample
 * 
 * This sample demonstrates how to pass data to a timer callback
 * function using the Zephyr kernel timer API.
 * A timer is created that, upon expiration, retrieves and prints
 * the data passed to it.
 */
#include <zephyr/kernel.h>

/* Define the functions used when the timer expired */
void expiry_callback( struct k_timer *timer_id );

/* declare a software timer called timer_id, and expiry_callback
with no stop callback function */
K_TIMER_DEFINE( timer_id, expiry_callback, NULL );

int main( void )
{
    static int count = 100;
    /* pass address of count variable to timer */
    k_timer_user_data_set( &timer_id, &count );
    /* start a timer that expires after 2 seconds and print 
    contents of count variable */
    k_timer_start( &timer_id, K_MSEC(2000), K_NO_WAIT );
}

void expiry_callback( struct k_timer *timer_id )
{
    /* retrieve address of count variable passed to timer
    and therefore the content */
    int *count = k_timer_user_data_get( timer_id );
    printk("Info passed to timer: %d\n", *count);
}