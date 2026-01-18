/**
 * @file main.c
 * @brief Single zbus channel with one observer of type subscriber
 * 
 * This sample code demonstrates how to define and register a single zbus channel
 * with one observer of type subscriber. another thread is been used to read the message
 * from the subscriber queue. This is pretty much the same as if we were using a message queue.
 */
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/*define message for channel 1*/
struct dummy_msg1 {
    uint8_t major;
    uint8_t minor;
};


/*define a single channel with one observer*/
ZBUS_CHAN_DEFINE(channel_1, struct dummy_msg1, NULL, NULL, ZBUS_OBSERVERS(subscriber_listener), ZBUS_MSG_INIT(0));

static void subscriber_task( void *p1, void *p2, void *p3 );

/*declare a task that will be used to read from the subscriber listener*/
K_THREAD_DEFINE( subscriber_thread_id, 1024, subscriber_task, NULL, NULL, NULL, 7, 0, 0 );

/*define a single subscriber with a size of 5 messages, remember subscriber are message queues*/
ZBUS_SUBSCRIBER_DEFINE( subscriber_listener, 5 );

int main(void)
{
    struct dummy_msg1 msg = { .major = 1, .minor = 0 };

    printk("Publishing messages to channel 1 every 2 seconds\n");
    
    while(true)
    {
        /*publish a message to channel 1, but this time the message is published in a 
        message queue to be read by the subscriber task*/
        msg.minor++;
        zbus_chan_pub( &channel_1, &msg, K_NO_WAIT );

        k_sleep(K_SECONDS(2));
    }

    return 0;
}

/*this is the actual subscriber task, where we are going to read a message like if it was send
in a message queue*/
static void subscriber_task( void *p1, void *p2, void *p3 )
{
    struct dummy_msg1 received_msg;

    while(1)
    {
        /*read the message from the channel that has sent the message*/
        zbus_chan_read(&channel_1, &received_msg, K_FOREVER);

        printk("Subscriber received message from channel: %s, major: %d, minor: %d, thread: %s\n", 
            zbus_chan_name(&channel_1), received_msg.major, received_msg.minor, k_thread_name_get(k_current_get()));
    }
}