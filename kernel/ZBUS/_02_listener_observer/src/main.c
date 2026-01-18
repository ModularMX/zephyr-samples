/**
 * @file main.c
 * @brief Single zbus channel with one observer of type listener
 * 
 * This sample code demonstrates how to define and register a single zbus channel
 * with one observer of type listener. It also shows how to publish messages to the
 * channel and have the listener callback invoked upon message publication.
 */
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/*define message for channel 1*/
struct dummy_msg1 {
    uint8_t major;
    uint8_t minor;
};


/*define a single channel with one observer*/
ZBUS_CHAN_DEFINE(channel_1, struct dummy_msg1, NULL, NULL, ZBUS_OBSERVERS(foo_listener), ZBUS_MSG_INIT(0));

static void listener_callback( const struct zbus_channel *chan );

/*define a single listener, with function listener_callback to be called when channel is updated*/
ZBUS_LISTENER_DEFINE( foo_listener, listener_callback );

int main(void)
{
    struct dummy_msg1 msg = { .major = 1, .minor = 0 };

    printk("Publishing messages to channel 1 every 2 seconds\n");
    
    while(true)
    {
        /*publish a message to channel 1*/
        msg.minor++;
        zbus_chan_pub( &channel_1, &msg, K_NO_WAIT );

        k_sleep(K_SECONDS(2));
    }

    return 0;
}

/*this function will be called whenever there is a new message published to channel_1
the callback is called from the same thread where the message has been published */
static void listener_callback( const struct zbus_channel *chan )
{
    const struct dummy_msg1 *v = zbus_chan_const_msg(chan);
    printk("Listener callback invoked for channel: %s, major: %d, minor: %d, thread: %s\n", 
        zbus_chan_name(chan), v->major, v->minor, k_thread_name_get(k_current_get()));
}