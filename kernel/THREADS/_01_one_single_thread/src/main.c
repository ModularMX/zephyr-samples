/**
 * @file main.c
 * @brief simple thread declare in build time
 * 
 * This sample creates a single thread that prints a message every second.
 * It demonstrates how to define and run a thread using Zephyr's kernel API.
 */
#include <zephyr/kernel.h>

/* Define the function used as thread */
void bottle_th( void *p1, void *p2, void *p3 );

/* Thread definition in build time, to run immediately, with 1024 bytes of stack 
and a priority of 7, no parameters, no options*/
K_THREAD_DEFINE( thread_id, 1024, bottle_th, NULL, NULL, NULL, 7, 0, 0 );

/* Function used as thread */
void bottle_th( void *p1, void *p2, void *p3 )
{
    while(1)
    {
        /* Toggle led0 */
        printk("message from thread\n");
        /* Delay of 1000ms */
        k_msleep(1000);
    }
}