/*
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/led.h>
#include <zephyr/random/random.h>

#include "device.h"

#define SENSOR_CHAN     SENSOR_CHAN_AMBIENT_TEMP
#define SENSOR_UNIT     "Celsius"

/* Devices */
static const struct device *sensor = DEVICE_DT_GET_OR_NULL(DT_ALIAS(ambient_temp0));
static const struct device *leds = DEVICE_DT_GET_OR_NULL(DT_INST(0, gpio_leds));

/* Command handlers */
static void led_on_handler(void)
{
	device_write_led(LED_USER, LED_ON);
}

static void led_off_handler(void)
{
	device_write_led(LED_USER, LED_OFF);
}

/* Supported device commands */
struct device_cmd device_commands[] = {
	{"led_on", led_on_handler},
	{"led_off", led_off_handler}
};

const size_t num_device_commands = ARRAY_SIZE(device_commands);

/* Command dispatcher */
void device_command_handler(uint8_t *command)
{
	for (int i = 0; i < num_device_commands; i++) {
		if (strcmp(command, device_commands[i].command) == 0) {
			printk("Executing device command: %s\n", device_commands[i].command);
			return device_commands[i].handler();
		}
	}
	printk("Unknown command: %s\n", command);
}

int device_read_sensor(struct sensor_sample *sample)
{
	int rc;
	struct sensor_value sensor_val;

	/* Read sample only if a real sensor device is present
	 * otherwise return a dummy value
	 */
	if (sensor == NULL) {
		sample->unit = SENSOR_UNIT;
		sample->value = 20.0 + (double)sys_rand32_get() / UINT32_MAX * 5.0;
		return 0;
	}

	rc = sensor_sample_fetch(sensor);
	if (rc) {
		printk("Failed to fetch sensor sample [%d]\n", rc);
		return rc;
	}

	rc = sensor_channel_get(sensor, SENSOR_CHAN, &sensor_val);
	if (rc) {
		printk("Failed to get sensor channel [%d]\n", rc);
		return rc;
	}

	sample->unit = SENSOR_UNIT;
	sample->value = sensor_value_to_double(&sensor_val);
	return rc;
}

int device_write_led(enum led_id led_idx, enum led_state state)
{
	int rc;

	switch (state) {
	case LED_OFF:
		if (leds == NULL) {
			printk("LED %d OFF\n", led_idx);
			break;
		}
		led_off(leds, led_idx);
		break;
	case LED_ON:
		if (leds == NULL) {
			printk("LED %d ON\n", led_idx);
			break;
		}
		led_on(leds, led_idx);
		break;
	default:
		printk("Invalid LED state setting\n");
		rc = -EINVAL;
		break;
	}

	return rc;
}

bool devices_ready(void)
{
	bool rc = true;

	/* Check readiness only if a real sensor device is present */
	if (sensor != NULL) {
		if (!device_is_ready(sensor)) {
			printk("Device %s is not ready\n", sensor->name);
			rc = false;
		} else {
			printk("Device %s is ready\n", sensor->name);
		}
	}

	if (leds != NULL) {
		if (!device_is_ready(leds)) {
			printk("Device %s is not ready\n", leds->name);
			rc = false;
		} else {
			printk("Device %s is ready\n", leds->name);
		}
	}

	return rc;
}
