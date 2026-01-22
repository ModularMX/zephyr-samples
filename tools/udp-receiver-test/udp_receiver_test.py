#!/usr/bin/env python3
"""
UDP Receiver Test - Simple UDP server for testing UDP sender

This script:
- Listens on 0.0.0.0:4242 (all interfaces)
- Receives UDP packets from STM32 UDP sender
- Displays sender IP, port, and packet data
- Shows packet counter

Usage:
    python udp_receiver_test.py

Expected output:
    [INFO] Starting UDP server on 0.0.0.0:4242
    [OK] Bound to 0.0.0.0:4242
    [INFO] Listening... (Ctrl+C to stop)
    [RX] From 192.168.1.100:51810 - 34 bytes
         Data: Hello from Zephyr UDP! [packet #0]
"""

import socket
import sys

HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 4242

print(f"[INFO] Starting UDP server on {HOST}:{PORT}", flush=True)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

try:
    sock.bind((HOST, PORT))
    print(f"[OK] Bound to {HOST}:{PORT}", flush=True)
except Exception as e:
    print(f"[ERROR] Bind failed: {e}", flush=True)
    sys.exit(1)

print(f"[INFO] Listening... (Ctrl+C to stop)", flush=True)

try:
    while True:
        data, addr = sock.recvfrom(1024)
        print(f"[RX] From {addr[0]}:{addr[1]} - {len(data)} bytes")
        print(f"     Data: {data.decode('utf-8', errors='replace')}")
except KeyboardInterrupt:
    print("\n[INFO] Shutting down")
finally:
    sock.close()
