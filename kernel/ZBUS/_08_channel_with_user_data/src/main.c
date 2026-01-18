/**
 * @file main.c
 * @brief Single zbus channel with user data in listener observer
 * 
 * This sample code demonstrates how to define and register a single zbus channel
 * with one observer of type listener but with user data added.
 */
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/*define message for channel 1*/
struct dummy_msg1 {
    uint8_t major;
    uint8_t minor;
};

/*user data structure*/
struct user_data {
    int some_value;
};

/*define some user data to be referenced by the message*/
static struct user_data udata = { .some_value = 42 };

/*define a single channel with user data*/
ZBUS_CHAN_DEFINE(channel_1, struct dummy_msg1, NULL, &udata, ZBUS_OBSERVERS(foo_listener), ZBUS_MSG_INIT(0));

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
    /*get the message*/
    const struct dummy_msg1 *v = zbus_chan_const_msg(chan);
    /*get the user data, just remember we are accesing through a reference, in case you like to modify it 
    you need to claim the channel first then relase it to avoid data corruption from some other observer
    accesing the user data*/
    struct user_data *udata = zbus_chan_user_data(chan);
    
    printk("Listener callback invoked for channel: %s, major: %d, minor: %d, user data: %d, thread: %s\n", 
        zbus_chan_name(chan), v->major, v->minor, udata->some_value, k_thread_name_get(k_current_get()));
}
