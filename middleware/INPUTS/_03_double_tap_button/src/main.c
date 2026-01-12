#include <zephyr/kernel.h>
#include <zephyr/input/input.h>

static void button_callback(struct input_event *evt, void *user_data)
{
	if (evt->type != INPUT_EV_KEY) {
		return;
	}

	/* Only respond to press events (value == 1) */
	if (evt->value != 1) {
		return;
	}

	switch (evt->code) {
	case INPUT_KEY_0:
		printk("Single tap detected\n");
		break;
	
	case INPUT_KEY_A:
		printk("DOUBLE TAP detected!\n");
		break;
	}
}

INPUT_CALLBACK_DEFINE(NULL, button_callback, NULL);

int main(void)
{
	printk("Double Tap Button Example\n");
	printk("Press the button twice quickly (within 300ms) to trigger double tap\n");
	printk("Waiting for button events...\n");
	
	return 0;
}
