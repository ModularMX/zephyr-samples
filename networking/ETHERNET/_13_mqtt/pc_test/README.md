#!/usr/bin/env python3
"""
README - MQTT Monitor for Zephyr IoT Device
"""

# MQTT Monitor - Test Script for Zephyr IoT Device

This script allows you to monitor and control the Zephyr MQTT IoT device from your PC.

## Installation

```powershell
pip install paho-mqtt
```

## Usage

Run the script:

```powershell
cd c:\Users\USER\Desktop\zephyrproject\apps\networking\ETHERNET\_13_mqtt\pc_test
python mqtt_monitor.py
```

## Features

- ✅ **Subscribe to sensor data** - Receives temperature/sensor readings from the device
- ✅ **Send commands** - Control LEDs and other device features
- ✅ **JSON parsing** - Automatically parses and displays JSON payloads
- ✅ **TLS encryption** - Secure connection to test.mosquitto.org

## Available Commands

| Command | Effect |
|---------|--------|
| `led_on` | Turn ON the USER LED |
| `led_off` | Turn OFF the USER LED |

## Configuration

Edit `mqtt_monitor.py` to change:

- `MQTT_BROKER` - Broker hostname (default: test.mosquitto.org)
- `MQTT_PORT` - Broker port (default: 8883 for TLS)
- `MQTT_PUB_TOPIC` - Sensor data topic
- `MQTT_CMD_TOPIC` - Command topic
- `USE_TLS` - Enable/disable TLS

## Using Local Mosquitto Broker

If you want to use a local broker instead of test.mosquitto.org:

1. Install Mosquitto:
```powershell
choco install mosquitto
```

2. Start Mosquitto:
```powershell
mosquitto -v
```

3. Edit `mqtt_monitor.py`:
```python
MQTT_BROKER = "localhost"
MQTT_PORT = 1883
USE_TLS = False
```

4. Also update firmware `mqtt_config.h`:
```c
#define MQTT_BROKER_HOSTNAME "192.168.0.100"  // Your PC IP
#define MQTT_BROKER_PORT "1883"
```

## Example Session

```
60
MQTT Monitor - Zephyr IoT Device
============================================================
Broker: test.mosquitto.org:8883
Sensor Topic: zephyr_sample/sensor
Command Topic: zephyr_sample/command
TLS: Yes
============================================================
✓ Connected to MQTT broker successfully
✓ Subscribed to 'zephyr_sample/sensor'

Commands:
  Type a command and press Enter to send
  Examples: 'led_on', 'led_off'
  Type 'quit' or 'exit' to disconnect

→ Enter command: led_on
✓ Sent command to 'zephyr_sample/command': led_on

📨 Message from 'zephyr_sample/sensor':
   Payload: {"unit":"Celsius","value":22.5}
   Parsed JSON: {
        "unit": "Celsius",
        "value": 22.5
   }
```
