/**
 * @file main.c
 * @brief Spawning threads in runtime sample
 * 
 * This sample demonstrates how to create and run threads dynamically in 
 * Zephyr OS using k_thread_create() API with also dynamic stack allocation.
 * It is important to configure the project to enable dynamic threads and
 * set the stack size and pool size accordingly in teh corresponding 
 * <board>.conf file.
 */
#include <zephyr/kernel.h>

/* Define the function used as thread */
void thread_func( void *p1, void *p2, void *p3 );

int main( void )
{
    /* Thread Control Blocks, should be alocated in a global memory space*/
    static struct k_thread thread_tcb0;
    static struct k_thread thread_tcb1;

    /* allocate 1024 bytes of memory from the heap pool for each the stack memory
    refer to <board>.conf options */
    k_thread_stack_t *thread_stack0 = k_thread_stack_alloc( CONFIG_DYNAMIC_THREAD_STACK_SIZE, 0 );
    k_thread_stack_t *thread_stack1 = k_thread_stack_alloc( CONFIG_DYNAMIC_THREAD_STACK_SIZE, 0 );


	/* Register the thread and run immediately */
	k_tid_t  thread_id0 = k_thread_create( &thread_tcb0, thread_stack0, K_THREAD_STACK_SIZEOF(thread_stack0),
                                   thread_func, "Thread 0", NULL, NULL, 5, 0, K_NO_WAIT);
    
    /* Register another thread and run after 2 seconds */
    k_tid_t  thread_id1 = k_thread_create( &thread_tcb1, thread_stack1, K_THREAD_STACK_SIZEOF(thread_stack1),
                                   thread_func, "Thread 1", NULL, NULL, 5, 0, K_SECONDS(2));

    (void)thread_id0;
    (void)thread_id1;
    
    return 0;
}

/* Define the function used as thread */
void thread_func( void *p1, void *p2, void *p3 )
{
    char *thread_name = (char *) p1;
	while(1) 
	{
        /*just print a message*/
        printk("Message from %s\r", thread_name);
		/* Send thread to sleep for 500ms */
		k_msleep( 1000 );
	}
}