/**
 * @file main.c
 * @brief Single zbus channel with two observers
 * 
 * This sample code demonstrates how to define and register a single zbus channel
 * with two observers. One oberser of type async listener and a second one subscriber.
 * The message is send to both observers when published every two seconds.
 * 
 * Async listener will receive the message first because it is called from the system work queue
 * and it has a higher priority than the subscriber thread.
 */
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/*define message for channel 1*/
struct dummy_msg1 {
    uint8_t major;
    uint8_t minor;
};


/*define a single channel with two observers*/
ZBUS_CHAN_DEFINE(channel_1, struct dummy_msg1, NULL, NULL, ZBUS_OBSERVERS(async_listener, msg_subscriber_listener), ZBUS_MSG_INIT(0));

static void msg_subscriber_task( void *p1, void *p2, void *p3 );
static void async_listener_callback( const struct zbus_channel *chan, const void *message );

/*declare a task that will be used to read from the subscriber listener*/
K_THREAD_DEFINE( msg_subscriber_thread_id, 1024, msg_subscriber_task, NULL, NULL, NULL, 7, 0, 0 );

/*define a single asynchronous listener, with function async_listener_callback 
to be called when channel is updated*/
ZBUS_ASYNC_LISTENER_DEFINE( async_listener, async_listener_callback );
/*define a single message subscriber with a size of 5 messages, remember message subscribers use FIFOs*/
ZBUS_MSG_SUBSCRIBER_DEFINE( msg_subscriber_listener );

int main(void)
{
    struct dummy_msg1 msg = { .major = 1, .minor = 0 };

    while(true)
    {
        printk("Publishing messages to channel 1 every 2 seconds\n");
        /*publish a message to channel 1, but this time the message is published in a 
        message FIFO to be read by the message subscriber task*/
        msg.minor++;
        zbus_chan_pub( &channel_1, &msg, K_NO_WAIT );

        k_sleep(K_SECONDS(2));
    }

    return 0;
}

/*this is the actual message subscriber task, where we are going to read a message like if it was send
in a message FIFO*/
static void msg_subscriber_task( void *p1, void *p2, void *p3 )
{
    const struct zbus_channel *chan;
    struct dummy_msg1 received_msg;

    while(1)
    {
        /*wait for a new message to be available in the message subscriber FIFO from any channel*/
        zbus_sub_wait_msg( &msg_subscriber_listener, &chan, &received_msg, K_FOREVER );
        
        printk("Message subscriber received message from channel: %s, major: %d, minor: %d, thread: %s\n", 
            zbus_chan_name(chan), received_msg.major, received_msg.minor, k_thread_name_get(k_current_get()));
    }
}

/*async listener callback function*/
static void async_listener_callback( const struct zbus_channel *chan, const void *message )
{
    const struct dummy_msg1 *msg = (const struct dummy_msg1 *)message;
    printk("Async listener received message from channel: %s, major: %d, minor: %d, thread: %s\n", 
        zbus_chan_name(chan), msg->major, msg->minor, k_thread_name_get(k_current_get()));
}