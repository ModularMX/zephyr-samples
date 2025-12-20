/**
 * @file main.c
 * @brief interaation between two threads
 * 
 * This sample creates two threads that print messages.
 * Thread 0 has a higher priority than thread 1. Every 5 iterations,
 * thread 0 suspends/resumes thread 1 to demonstrate thread control
 */
#include <zephyr/kernel.h>

/* Define the functions used as thread */
void bottle_th0( void *p1, void *p2, void *p3 );
void bottle_th1( void *p1, void *p2, void *p3 );

/* Thread definitions, two of them with same stack value size but different
 priority thread 0 has a higher priority than thread 1 */
K_THREAD_DEFINE( thread_id0, 1024, bottle_th0, NULL, NULL, NULL, 7, 0, 0 );
K_THREAD_DEFINE( thread_id1, 1024, bottle_th1, NULL, NULL, NULL, 8, 0, 0 );

/* Function used as thread */
void bottle_th0( void *p1, void *p2, void *p3 )
{
    int i = 0;
    uint8_t flag = 0;
    while(1)
    {
        /* print count */
        printk("message from thread 0: %d\n", i++);

        /* every 5 iterations suspend/resume thread 1 */
        if(i % 5 == 0)
        {
            if(flag == 0){
                k_thread_suspend(thread_id1);
                printk("suspending thread 1\n");
            }
            else{
                k_thread_resume(thread_id1);
                printk("resuming thread 1\n");
            }
            flag ^= 1;
        }
        /* Delay of 1000ms */
        k_msleep(1000);
    }
}

void bottle_th1( void *p1, void *p2, void *p3 )
{
    int i = 0;
    while(1)
    {
        i %= 5;
        /* print count */
        printk("message from thread 1: %d\n", i++);
        /* Delay of 1000ms */
        k_msleep(1000);
    }
}