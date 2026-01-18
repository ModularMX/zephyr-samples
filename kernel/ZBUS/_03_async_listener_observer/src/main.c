/**
 * @file main.c
 * @brief Single zbus channel with one observer of type async listener
 * 
 * This sample code demonstrates how to define and register a single zbus channel
 * with one observer of type async listener. It is important to notice the callback is 
 * actually a work queue message sended to the system work queue thread.
 * 
 * It is necesary to enable CONFIG_ZBUS_ASYNC_LISTENER in prj.conf to use this type
 * of observer.
 */
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/*define message for channel 1*/
struct dummy_msg1 {
    uint8_t major;
    uint8_t minor;
};


/*define a single channel with one observer*/
ZBUS_CHAN_DEFINE(channel_1, struct dummy_msg1, NULL, NULL, ZBUS_OBSERVERS(async_listener), ZBUS_MSG_INIT(0));

static void async_listener_callback( const struct zbus_channel *chan, const void *message );

/*define a single asynchronous listener, with function async_listener_callback 
to be called when channel is updated*/
ZBUS_ASYNC_LISTENER_DEFINE( async_listener, async_listener_callback );

int main(void)
{
    struct dummy_msg1 msg = { .major = 1, .minor = 0 };

    printk("Publishing messages to channel 1 every 2 seconds\n");
    
    while(true)
    {
        /*publish a message to channel 1, but this time the message is published asynchronously
        meaning the listener callback will be called from the system work queue thread*/
        msg.minor++;
        zbus_chan_pub( &channel_1, &msg, K_NO_WAIT );

        k_sleep(K_SECONDS(2));
    }

    return 0;
}

/*this function will be called from the system work queue thread whenever there is a 
new message published to channel_1*/
static void async_listener_callback( const struct zbus_channel *chan, const void *message )
{
    const struct dummy_msg1 *v = message;
    printk("Listener callback invoked for channel: %s, major: %d, minor: %d, thread: %s\n", 
        zbus_chan_name(chan), v->major, v->minor, k_thread_name_get(k_current_get()));
}