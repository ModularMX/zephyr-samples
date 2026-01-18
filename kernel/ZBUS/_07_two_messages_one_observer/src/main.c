/**
 * @file main.c
 * @brief Two zbus channels with one observer
 * 
 * This sample code demonstrates how to define and register two zbus channels
 * with one observer. The observer is of type subscriber. The thread reads messages
 * from both channels and to distinguish them we can check the channel pointer against
 * the channel variables returned by zbus_sub_wait function.
 */
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/*define message for channel 1*/
struct dummy_msg1 {
    uint8_t major;
    uint8_t minor;
};

struct dummy_msg2 {
    uint16_t voltage;
    uint16_t current;
};

/*define a single channel with two observers*/
ZBUS_CHAN_DEFINE(channel_1, struct dummy_msg1, NULL, NULL, ZBUS_OBSERVERS(subscriber_listener), ZBUS_MSG_INIT(0));
ZBUS_CHAN_DEFINE(channel_2, struct dummy_msg2, NULL, NULL, ZBUS_OBSERVERS(subscriber_listener), ZBUS_MSG_INIT(0));
    
static void subscriber_task( void *p1, void *p2, void *p3 );

/*declare a task that will be used to read from the subscriber listener*/
K_THREAD_DEFINE( subscriber_thread_id, 1024, subscriber_task, NULL, NULL, NULL, 7, 0, 0 );

/*define a single message subscriber with a size of 5 messages, remember message subscribers use FIFOs*/
ZBUS_SUBSCRIBER_DEFINE( subscriber_listener, 5 );

int main(void)
{
    struct dummy_msg1 msg = { .major = 1, .minor = 0 };
    struct dummy_msg2 msg2 = { .voltage = 220, .current = 10 };

    while(true)
    {
        printk("Publishing two messages to channel 1 and channel 2 every 2 seconds\n");
        /*publish a message to channel 1, to be read by the message subscriber task*/
        msg.minor++;
        zbus_chan_pub( &channel_1, &msg, K_NO_WAIT );
        /*publish a message to channel 2, to be read by the message subscriber task*/
        msg2.current++;
        zbus_chan_pub( &channel_2, &msg2, K_NO_WAIT );

        k_sleep(K_SECONDS(2));
    }

    return 0;
}

static void subscriber_task( void *p1, void *p2, void *p3 )
{
    const struct zbus_channel *chan;
    struct dummy_msg1 received_msg;
    struct dummy_msg2 received_msg2;

    while(1)
    {
        /*wait for a new message to be available in the subscriber queue from any channel
        we can avoid the use of this funtion if we are only using one channel*/
        zbus_sub_wait( &subscriber_listener, &chan, K_FOREVER );
        
        if( chan == &channel_1 ){
            /*read the message from the channel that has sent the message*/
            zbus_chan_read(chan, &received_msg, K_NO_WAIT);

            printk("Subscriber received message from channel: %s, major: %d, minor: %d, thread: %s\n", 
                zbus_chan_name(chan), received_msg.major, received_msg.minor, k_thread_name_get(k_current_get()));
            continue;
        }
        /*it is channel 2 then*/
        else if(chan == &channel_2 ){
            /*read the message from the channel that has sent the message*/
            zbus_chan_read(chan, &received_msg2, K_NO_WAIT);

            printk("Subscriber received message from channel: %s, voltage: %d, current: %d, thread: %s\n", 
                zbus_chan_name(chan), received_msg2.voltage, received_msg2.current, k_thread_name_get(k_current_get()));
        }
    }
}

