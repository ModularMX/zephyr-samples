/**
 * @file main.c
 * @brief Simple Event Sample Code
 * 
 * This sample demonstrates the use of Zephyr's event API to signal and 
 * wait for events between threads. The single envent used in this example  
 * is represented by bit 0 of a 32-bit event object, the rest of the bits are unused.
 * For the reason it is safet o use k_event_set() 
 */
#include <zephyr/kernel.h>

/*bit 0 will be represent our only dummy event*/
#define DUMMY_EVENT BIT(0)

/* Define the event object */
K_EVENT_DEFINE(my_event);

static void consumer_thread( void *p1, void *p2, void *p3 );
static void print_binary(uint32_t num);

K_THREAD_DEFINE(thread_id, 1024, consumer_thread, NULL, NULL, NULL, 7, 0, 0);

int main( void )
{
    while(1)
    {
        /*set a new dummy event every second*/
        k_event_set(&my_event, DUMMY_EVENT);
        k_msleep(1000);
    }
    
    return 0;
}

static void consumer_thread( void *p1, void *p2, void *p3 )
{
    uint32_t event;
    
    while(1)
    {
        /*wait for the dummy event to be set, then clear the event (bit 0), 
        if for some reason we no need to clear the event, set the third parameter 
        to false*/
        event = k_event_wait(&my_event, DUMMY_EVENT, true, K_FOREVER);
        
        printk("\nEvent DUMMY_EVENT received!\n");
        print_binary(event);
    }
}

/*function to print a 32 bit number in binary notation, 
with a separation with an space every 4 digits*/
static void print_binary(uint32_t num)
{
    for (int i = 31; i >= 0; i--) {
        printk("%d", (num >> i) & 1);
        if (i % 4 == 0 && i != 0) {
            printk(" ");
        }
    }
    printk("\n");
}