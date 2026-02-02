# MQTT IoT Device Example

Zephyr device that connects to an MQTT broker, publishes sensor data, and receives remote commands.

## Core Components

- **main.c** - Network initialization (DHCP) and MQTT integration
- **mqtt_client.c** - MQTT client with TLS, publish/subscribe
- **device.c** - Sensor and LED control
- **mqtt_config.h** - Hardcoded configuration: broker, topics, interval
- **pc_test/mqtt_monitor.py** - Python tool to monitor and send commands from PC

## Basic Flow

1. Device boots
2. DHCP assigns IP (e.g., 192.168.0.18)
3. DNS resolves test.mosquitto.org
4. TLS connects to broker (port 8883)
5. Subscribes to "zephyr_sample/command"
6. Publishes sensor data every 3 seconds to "zephyr_sample/sensor"
7. Receives and executes commands (led_on, led_off)

## MQTT Topics

| Topic | Type | Data |
|-------|------|------|
| zephyr_sample/sensor | Publish | {"unit":"Celsius","value":22.5} |
| zephyr_sample/command | Subscribe | led_on / led_off |

## Configuration (mqtt_config.h)

```c
#define MQTT_BROKER_HOSTNAME    "test.mosquitto.org"
#define MQTT_BROKER_PORT        "8883"  // TLS
#define MQTT_PUB_TOPIC          "zephyr_sample/sensor"
#define MQTT_SUB_TOPIC_CMD      "zephyr_sample/command"
#define MQTT_QOS                1       // At Least Once
#define MQTT_PUBLISH_INTERVAL   3       // seconds
#define MQTT_PAYLOAD_SIZE       128     // bytes
```

Edit these values to change broker, topics, or interval.

## Build

```powershell
west build -b stm32h573i_dk apps/networking/ETHERNET/_13_mqtt
west flash
```

## Monitor from PC

```powershell
cd apps/networking/ETHERNET/_13_mqtt/pc_test
python mqtt_monitor.py
```

Enter commands interactively:
- `led_on` - Turn LED on
- `led_off` - Turn LED off
- `quit` - Disconnect

## Expected Serial Output

```
IPv4 address assigned - network ready!
Waiting for network configuration (DHCP)...
IPv4 address already assigned!
Network ready, initializing MQTT...
Connecting to MQTT broker @ 54.36.178.49
Connected to MQTT broker!
Hostname: test.mosquitto.org
Client ID: stm32h573i_dk_2e
Port: 8883
TLS: Enabled
Subscribing to 1 topic(s)
Published to topic 'zephyr_sample/sensor', QoS 1
PUBACK packet ID: 1
```

## QoS Levels

- **QoS 0** - At Most Once: No confirmation
- **QoS 1** - At Least Once: Waits for PUBACK (default)
- **QoS 2** - Exactly Once: Waits for PUBCOMP

Use QoS 1 to guarantee delivery without unnecessary duplicates.

## TLS/Security

Port 8883 uses TLS 1.2/1.3. CA certificates are auto-downloaded from the broker.

For local broker without TLS:
- Edit mqtt_config.h: MQTT_BROKER_HOSTNAME = "192.168.0.100", MQTT_BROKER_PORT = "1883"
- In prj.conf: CONFIG_MQTT_LIB_TLS=n


## Notes

- Automatic DHCP via net_config
- TLS certificates auto-downloaded
- Work queue for periodic publishing (non-blocking)
- Sensor can be real or simulated (random values)
