#!/usr/bin/env python3
"""
MQTT Monitor - Subscribe to Zephyr device topics and send commands
"""

import paho.mqtt.client as mqtt
import json
import time
import sys
import os

# MQTT Configuration (must match mqtt_config.h in firmware)
MQTT_BROKER = "test.mosquitto.org"
MQTT_PORT = 8883  # TLS
MQTT_PUB_TOPIC = "zephyr_sample/sensor"
MQTT_CMD_TOPIC = "zephyr_sample/command"

# TLS Configuration
USE_TLS = True
# Path to certificate file (downloaded from test.mosquitto.org)
CERT_PATH = os.path.join(os.path.dirname(__file__), "mosquitto_org.crt")

client = None

def on_connect(client, userdata, connect_flags, reason_code, properties):
    """Callback when connecting to broker (VERSION2 API)"""
    if reason_code == 0:
        print("✓ Connected to MQTT broker successfully")
        # Subscribe to sensor data topic
        client.subscribe(MQTT_PUB_TOPIC)
        print(f"✓ Subscribed to '{MQTT_PUB_TOPIC}'")
    else:
        print(f"✗ Failed to connect, reason code {reason_code}")

def on_disconnect(client, userdata, disconnect_flags, reason_code, properties):
    """Callback when disconnecting from broker (VERSION2 API)"""
    if reason_code != 0:
        print(f"✗ Unexpected disconnection from broker (code {reason_code})")
    else:
        print("✓ Disconnected from broker")

def on_message(client, userdata, msg, *args):
    """Callback when receiving a message (VERSION2 API)"""
    print(f"\n📨 Message from '{msg.topic}':")
    print(f"   Payload: {msg.payload.decode()}")
    
    # Try to parse as JSON
    try:
        data = json.loads(msg.payload.decode())
        print(f"   Parsed JSON: {json.dumps(data, indent=6)}")
    except:
        pass
    
    print()

def send_command(command_str):
    """Send a command to the device"""
    try:
        global client
        client.publish(MQTT_CMD_TOPIC, command_str, qos=1)
        print(f"✓ Sent command to '{MQTT_CMD_TOPIC}': {command_str}\n")
    except Exception as e:
        print(f"✗ Failed to send command: {e}\n")

def main():
    """Main function"""
    global client
    
    print("=" * 60)
    print("MQTT Monitor - Zephyr IoT Device")
    print("=" * 60)
    print(f"Broker: {MQTT_BROKER}:{MQTT_PORT}")
    print(f"Sensor Topic: {MQTT_PUB_TOPIC}")
    print(f"Command Topic: {MQTT_CMD_TOPIC}")
    print(f"TLS: {'Yes' if USE_TLS else 'No'}")
    print("=" * 60)
    
    # Create MQTT client - use VERSION2 to avoid deprecation warning
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message
    
    # Configure TLS
    if USE_TLS:
        # Check if certificate file exists
        if os.path.exists(CERT_PATH):
            print(f"✓ Using certificate: {CERT_PATH}")
            client.tls_set(ca_certs=CERT_PATH, certfile=None, keyfile=None, 
                          cert_reqs=mqtt.ssl.CERT_REQUIRED)
            client.tls_insecure_set(False)
        else:
            print(f"⚠ Certificate not found: {CERT_PATH}")
            print("  Attempting to download...")
            try:
                import urllib.request
                os.makedirs(os.path.dirname(CERT_PATH), exist_ok=True)
                urllib.request.urlretrieve(
                    "https://test.mosquitto.org/ssl/mosquitto.org.crt",
                    CERT_PATH
                )
                print(f"✓ Downloaded certificate to: {CERT_PATH}")
                client.tls_set(ca_certs=CERT_PATH, certfile=None, keyfile=None,
                              cert_reqs=mqtt.ssl.CERT_REQUIRED)
                client.tls_insecure_set(False)
            except Exception as e:
                print(f"✗ Failed to download certificate: {e}")
                print("  Falling back to insecure connection...")
                client.tls_set(ca_certs=None, certfile=None, keyfile=None, 
                              cert_reqs=mqtt.ssl.CERT_NONE)
                client.tls_insecure_set(True)
    
    # Connect to broker
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
    except Exception as e:
        print(f"✗ Connection failed: {e}")
        return
    
    # Start network loop in background
    client.loop_start()
    
    print("\nCommands:")
    print("  Type a command and press Enter to send")
    print("  Examples: 'led_on', 'led_off'")
    print("  Type 'quit' or 'exit' to disconnect\n")
    
    try:
        while True:
            try:
                cmd = input("→ Enter command: ").strip()
                
                if not cmd:
                    continue
                
                if cmd.lower() in ['quit', 'exit']:
                    print("\nDisconnecting...")
                    break
                
                send_command(cmd)
                
            except KeyboardInterrupt:
                print("\n\nDisconnecting...")
                break
            except Exception as e:
                print(f"✗ Error: {e}\n")
    
    finally:
        client.loop_stop()
        client.disconnect()
        print("✓ Disconnected")

if __name__ == "__main__":
    main()
