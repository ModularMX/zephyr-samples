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
void timer1_callback( struct k_timer *timer_id );
void timer2_callback( struct k_timer *timer_id );
/* Define the function used when the timer is stopped */
void stop_callback( struct k_timer *timer_id );

/* declare a software timers, timer 1 will have also a stop callback function */
K_TIMER_DEFINE( timer_id1, timer1_callback, stop_callback );
K_TIMER_DEFINE( timer_id2, timer2_callback, NULL );

int main( void )
{

    /* start a periodic timer that expires at every 500ms */
    k_timer_start( &timer_id1, K_MSEC(500), K_MSEC(500) );
    /* start a one shot timer that expires after 3000ms */
    k_timer_start( &timer_id2, K_MSEC(3000), K_NO_WAIT );
}

void timer1_callback( struct k_timer *timer_id )
{
    /* This function is called each time timer1 expires */
    static int count = 0;
    printk("Timer 1 expired %d times\n", count++);
}

void stop_callback( struct k_timer *timer_id )
{
    /* This function is called when timer1 is stopped by timer2 */
    printk("Timer 1 stopped\n");
}

void timer2_callback( struct k_timer *timer_id )
{
    /* This function is called when timer2 expires */
    printk("Timer 2 expired and stopping timer1\n");
    k_timer_stop( &timer_id1 );
}