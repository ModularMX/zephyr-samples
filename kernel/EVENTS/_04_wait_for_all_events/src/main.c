/**
 * @file main.c
 * @brief Wait for all events
 * 
 * This sample demonstrates how to wait untill al expected events are set
 * in an event object using k_event_wait_all() function.
 */
#include <zephyr/kernel.h>

/*bit 0 will be represent our only dummy event*/
#define DUMMY_EVENT_0 BIT(0)
#define DUMMY_EVENT_3 BIT(3)

/* Define the event object */
K_EVENT_DEFINE(my_event);

static void consumer_thread( void *p1, void *p2, void *p3 );
static void print_binary(uint32_t num);

K_THREAD_DEFINE(thread_id, 1024, consumer_thread, NULL, NULL, NULL, 7, 0, 0);

int main( void )
{
    while(1)
    {
        /*set the first dummy event and wait 100ms*/
        k_event_post(&my_event, DUMMY_EVENT_0);
        k_msleep(100);
        /*set the second dummy event without clearing the first one*/
        k_event_post(&my_event, DUMMY_EVENT_3);
        
        k_msleep(1000);
    }
    
    return 0;
}

static void consumer_thread( void *p1, void *p2, void *p3 )
{
    uint32_t event;
    
    while(1)
    {
        /*wait for any of all the dummy events to be set, (bit 0 or 3), main task
        does not post all of them inmediatly but wait for 100ms between each post,
        just for demostration purpose we set the third parameter to false*/
        event = k_event_wait_all(&my_event, DUMMY_EVENT_0 | DUMMY_EVENT_3, false, K_FOREVER);

        printk("All Events received! Event value in binary: ");
        print_binary(event);

        /*process each of the events*/
        if(event & DUMMY_EVENT_0){
            printk("Dummy Event 0 received!\n");
        }
        if(event & DUMMY_EVENT_3){
            printk("Dummy Event 3 received!\n");
        }

        /*since we decided not to clear the received events at the moment
        we received them, we need to use k_event_clear() function*/
        k_event_clear(&my_event, event);
        
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