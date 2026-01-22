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
        /*set the first dummy event*/
        k_event_post(&my_event, DUMMY_EVENT_0);
        /*set the second dummy event without clearing the first one, this can not be achived
        using k_event_set()*/
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
        /*wait for any of the dummy events to be set, then clear the event (bit 0 or 3),
        just for demostration purpose we set the third parameter to false*/
        event = k_event_wait_safe(&my_event, DUMMY_EVENT_0 | DUMMY_EVENT_3, false, K_FOREVER);

        printk("Consumer Thread: Event received! Event value in binary: ");
        print_binary(event);

        /*check which event was received, in advance we know that both are received, but
        in this case we need to check each one separately and proceed accordingly*/
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