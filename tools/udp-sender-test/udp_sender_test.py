#!/usr/bin/env python3
"""
UDP Sender Test Script

Sends UDP packets to STM32 board UDP receiver.

Usage:
    python udp_sender_test.py
"""

import socket
import time
import sys

# Configuration
RECEIVER_IP = "192.168.1.100"  # STM32 board IP
RECEIVER_PORT = 4242
MESSAGE = "Hello from Python UDP!"
NUM_PACKETS = 5
INTERVAL_SECONDS = 2

def send_udp_packets():
    """Send UDP packets to the receiver"""
    
    print(f"[INFO] UDP Sender Test")
    print(f"[INFO] Target: {RECEIVER_IP}:{RECEIVER_PORT}")
    print(f"[INFO] Sending {NUM_PACKETS} packets every {INTERVAL_SECONDS}s")
    print("-" * 60)
    
    try:
        # Create UDP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        print(f"[OK] Socket created", flush=True)
        
        # Send packets
        for i in range(NUM_PACKETS):
            # Create message with packet counter (same format as Zephyr sender)
            message = f"{MESSAGE} [packet #{i}]"
            message_bytes = message.encode('utf-8')
            
            # Send packet
            sock.sendto(message_bytes, (RECEIVER_IP, RECEIVER_PORT))
            
            print(f"[TX] Packet #{i} sent ({len(message_bytes)} bytes)", flush=True)
            print(f"     Data: '{message}'", flush=True)
            
            # Wait before sending next packet (except for last one)
            if i < NUM_PACKETS - 1:
                time.sleep(INTERVAL_SECONDS)
        
        print("-" * 60)
        print(f"[OK] Sent {NUM_PACKETS} packets successfully", flush=True)
        
        # Close socket
        sock.close()
        
    except KeyboardInterrupt:
        print("\n[INFO] Interrupted by user", flush=True)
        sock.close()
        sys.exit(0)
        
    except Exception as e:
        print(f"[ERROR] {e}", flush=True)
        sys.exit(1)

if __name__ == "__main__":
    send_udp_packets()
