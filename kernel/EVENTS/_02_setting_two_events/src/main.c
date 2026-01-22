/**
 * @file main.c
 * @brief Sample application setting and waiting for two events
 * 
 * This sample demonstrates how to set and wait for two events, In this example 
 * we define two dummy events, DUMMY_EVENT_0 and DUMMY_EVENT_2, represented 
 * by bit 0 and bit 2 respectively. A thread is created that waits for either 
 * of the events to be set. The main function sets both events every second.
 */
#include <zephyr/kernel.h>

/*bit 0 will be represent our only dummy event*/
#define DUMMY_EVENT_0   BIT(0)
#define DUMMY_EVENT_2   BIT(2)

/* Define the event object */
K_EVENT_DEFINE(my_event);

static void consumer_thread( void *p1, void *p2, void *p3 );
static void print_binary(uint32_t num);

K_THREAD_DEFINE(thread_id, 1024, consumer_thread, NULL, NULL, NULL, 7, 0, 0);

int main( void )
{
    while(1)
    {
        /*set two new dummy events every second, the function set always rewrites 
        the event value, make it not possible to use if we want to set multiple 
        events separately*/
        k_event_set(&my_event, DUMMY_EVENT_0 | DUMMY_EVENT_2);
        k_msleep(1000);
    }
    
    return 0;
}

static void consumer_thread( void *p1, void *p2, void *p3 )
{
    uint32_t event;
    
    while(1)
    {
        /*wait for any fo the dummy events to be set, then clear the event (bit 0 or bit 2), 
        if for some reason we no need to clear the event, set the third parameter 
        to false*/
        event = k_event_wait(&my_event, DUMMY_EVENT_0 | DUMMY_EVENT_2, true, K_FOREVER);
        
        /*in this example we always set both events at the same time so is expected
        to always receive both, both this is not always the case*/
        if(event & DUMMY_EVENT_0){
            printk("Event DUMMY_EVENT_0 received!\n");
        }
        if(event & DUMMY_EVENT_2){
            printk("Event DUMMY_EVENT_2 received!\n");
        }
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