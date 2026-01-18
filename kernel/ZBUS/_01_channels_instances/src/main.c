/**
 * @file main.c
 * @brief register three zbus channels
 * 
 * This sample code demonstrates how to define and register three zbus channels
 * with different message types, initial values and no obersevers. It also shows 
 * how to iterate over all registered channels and print some of their information.
 * 
 * To get the channel name it is necesary to enable CONFIG_ZBUS_CHANNEL_NAME in
 * the project configuration file.
 */
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/*define message for channel 1*/
struct dummy_msg1 {
    uint8_t major;
    uint8_t minor;
};

/*define message for channel 2*/
struct dummy_msg2 {
    int16_t x;
    int16_t y;
    int16_t z;
};

/*define three channels with no observer, and some values to set a inital message value*/
ZBUS_CHAN_DEFINE(channel_1, struct dummy_msg1, NULL, NULL, ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(0));
ZBUS_CHAN_DEFINE(channel_2, struct dummy_msg2, NULL, NULL, ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(.x=5, .y=10, .z=15));
ZBUS_CHAN_DEFINE(channel_3, int, NULL, NULL, ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(100));

static bool print_channels_iterator(const struct zbus_channel *chan, void *user_data);

int main(void)
{
    printk("Listing all ZBUS channels:\n");
    
    /* Use iterator to print all channels information, name and message size */
    int count = 0;
    zbus_iterate_over_channels_with_user_data(print_channels_iterator, &count);

    return 0;
}

/*this function will be called at least three times by zbus_iterate_over_channels_with_user_data
because there are three channels defined*/
static bool print_channels_iterator(const struct zbus_channel *chan, void *user_data)
{
    int *count = user_data;

    /* Print channel name and message size */
    printk("%d - Channel %s:\n", *count, zbus_chan_name(chan));
    printk("    Message size: %d\n", zbus_chan_msg_size(chan));

    ++(*count);

    /*we always return true to continue iterating over all channels, if for some reason 
    we would like to stop iterating over all channels, we would return false */
    return true;
}
