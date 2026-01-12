#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>

/*
 * Input event callback
 *
 * The zephyr,input-longpress driver automatically generates different codes:
 * - INPUT_KEY_A for short press
 * - INPUT_KEY_X for long press
 */
static void button_event_cb(struct input_event *evt, void *user_data)
{
    ARG_UNUSED(user_data);
    
    if (evt->type == INPUT_EV_KEY)
    {
        switch (evt->code)
        {
        case INPUT_KEY_A:  /* Short press event */
            if (evt->value) {
                printk(">>> SHORT PRESS detected <<<\n");
            }
            break;
            
        case INPUT_KEY_X:  /* Long press event */
            if (evt->value) {
                printk(">>> LONG PRESS detected <<<\n");
            }
            break;
            
        default:
            /* Ignore other events */
            break;
        }
    }
}

INPUT_CALLBACK_DEFINE(NULL, button_event_cb, NULL);

int main(void)
{
    printk("\n========================================\n");
    printk("  Long Press Detection Demo\n");
    printk("  Using: zephyr,input-longpress driver\n");
    printk("  Threshold: 1000 ms\n");
    printk("========================================\n\n");
    
    printk("System ready. Press the button...\n\n");

    while (1)
    {
        k_sleep(K_FOREVER);
    }
}
