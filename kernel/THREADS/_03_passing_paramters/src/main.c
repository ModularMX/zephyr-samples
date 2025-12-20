/**
 * @file main.c
 * @brief Application that demonstrates passing parameters to threads.
 * 
 * This sample creates two threads that print messages at different intervals.
 * Each thread receives a string message and a delay time as parameters.
 */
#include <zephyr/kernel.h>

/* Define the function used as thread */
void thread_func( void *p1, void *p2, void *p3 );

/* Thread definition in build time, to run immediately, with 1024 bytes of stack 
and a priority of 7, no parameters, no options*/
K_THREAD_DEFINE( thread0_id, 1024, thread_func, "thread 0", 1000, NULL, 7, 0, 0 );
K_THREAD_DEFINE( thread1_id, 1024, thread_func, "thread 1", 2000, NULL, 8, 0, 0 );

/* Function used as thread */
void thread_func( void *p1, void *p2, void *p3 )
{
    uint8_t *str = (uint8_t *)p1;
    
    while(1)
    {
        printk("message from thread: %s\n", str);
        /* Delay of ms indicated in paramter p2 */
        k_msleep( (int32_t)p2 );
    }
}