#!/usr/bin/env python3
"""
TCP Client for testing Zephyr TCP Server (Example 6)

This script creates a simple TCP client that:
1. Connects to a Zephyr board running TCP server
2. Sends multiple test messages
3. Receives and displays echoed responses
4. Shows connection statistics

Usage:
    python3 tcp_client.py

Configuration:
    SERVER_IP - Server IP address (default: 192.168.1.100)
    SERVER_PORT - Server port (default: 5555)
    MESSAGES - List of test messages to send
"""

import socket
import sys
import time

# Configuration
SERVER_IP = '192.168.1.100'
SERVER_PORT = 5555

# Test messages
MESSAGES = [
    b'Hello from TCP client!',
    b'Test message 1',
    b'Test message 2',
    b'Zephyr TCP Echo',
]

def main():
    # Create socket
    try:
        print("[INFO] TCP Client for Zephyr Server")
        print(f"[INFO] Connecting to {SERVER_IP}:{SERVER_PORT}...\n")
        
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER_IP, SERVER_PORT))
        
        print(f"[CONNECTED] Successfully connected to {SERVER_IP}:{SERVER_PORT}\n")
        
        # Send messages and receive echoes
        for i, message in enumerate(MESSAGES, 1):
            try:
                # Send message
                print(f"[{i}] Sending: {message.decode('utf-8')}")
                sock.send(message)
                
                # Receive echo
                response = sock.recv(1024)
                
                if response:
                    print(f"[{i}] Received: {response.decode('utf-8')}")
                    
                    # Verify echo matches
                    if response == message:
                        print(f"[{i}] ✓ Echo verified!\n")
                    else:
                        print(f"[{i}] ✗ Echo mismatch! Expected: {message}\n")
                else:
                    print(f"[{i}] Server closed connection\n")
                    break
                
                # Wait between messages
                time.sleep(1)
                
            except Exception as e:
                print(f"[ERROR] Error sending/receiving message {i}: {e}\n")
                break
        
        # Close connection
        sock.close()
        print("[INFO] Connection closed")
        print("[INFO] Test completed successfully!")
        
    except ConnectionRefusedError:
        print(f"[ERROR] Connection refused!")
        print(f"[ERROR] Make sure:")
        print(f"[ERROR]   - Zephyr board is flashed with Example 6 (TCP Server)")
        print(f"[ERROR]   - Board is reachable at {SERVER_IP}:{SERVER_PORT}")
        print(f"[ERROR]   - Network is configured correctly")
        sys.exit(1)
        
    except socket.timeout:
        print(f"[ERROR] Connection timeout!")
        print(f"[ERROR] Server at {SERVER_IP}:{SERVER_PORT} did not respond")
        sys.exit(1)
        
    except Exception as e:
        print(f"[ERROR] Unexpected error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
