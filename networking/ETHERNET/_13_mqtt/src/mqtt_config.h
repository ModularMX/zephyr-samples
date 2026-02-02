/*
 * MQTT Application Configuration
 * Hardcoded values - no Kconfig needed
 */

#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

/* MQTT Broker Configuration */
#define MQTT_BROKER_HOSTNAME "test.mosquitto.org"
#define MQTT_BROKER_PORT "8883"

/* MQTT Topics */
#define MQTT_PUB_TOPIC "zephyr_sample/sensor"
#define MQTT_SUB_TOPIC_CMD "zephyr_sample/command"

/* MQTT Quality of Service (0, 1, or 2) */
#define MQTT_QOS 1

/* Publish interval in seconds */
#define MQTT_PUBLISH_INTERVAL 3

/* MQTT payload buffer size in bytes */
#define MQTT_PAYLOAD_SIZE 128

#endif /* MQTT_CONFIG_H */
