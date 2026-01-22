#!/usr/bin/env python3
"""
TCP Echo Server for TCP Client Testing

This script creates a simple TCP echo server that:
1. Listens on 0.0.0.0:4242
2. Accepts connections from TCP clients
3. Receives data and echoes it back
4. Displays all communication on the console

Usage:
    python3 tcp_echo_server.py

TCP clients will connect to this server and send messages.
This server will echo them back.

Exit: Press Ctrl+C to stop the server
"""

import socket
import sys

def main():
    # Server configuration
    HOST = '0.0.0.0'  # Listen on all interfaces
    PORT = 4242       # Must match SERVER_PORT in Example 5
    BUFFER_SIZE = 1024

    # Create socket
    try:
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print(f"[INFO] TCP Echo Server")
        print(f"[INFO] Binding to {HOST}:{PORT}...")
        
        server_socket.bind((HOST, PORT))
        server_socket.listen(1)
        print(f"[INFO] Listening on {HOST}:{PORT}")
        print(f"[INFO] Waiting for connections from TCP clients...")
        print(f"[INFO] Press Ctrl+C to exit\n")

    except OSError as e:
        print(f"[ERROR] Failed to create server socket: {e}")
        sys.exit(1)

    try:
        while True:
            # Accept connection
            try:
                client_socket, client_address = server_socket.accept()
                print(f"[CONNECTED] Client connected from {client_address[0]}:{client_address[1]}")

                # Receive data
                data = client_socket.recv(BUFFER_SIZE)
                if data:
                    message = data.decode('utf-8', errors='replace')
                    print(f"[RECEIVED] ({len(data)} bytes): '{message}'")

                    # Echo data back
                    print(f"[SENDING] Echoing back: '{message}'")
                    client_socket.sendall(data)
                    print(f"[SENT] Echo sent successfully\n")

                # Close connection
                client_socket.close()
                print(f"[DISCONNECTED] Client disconnected\n")

            except Exception as e:
                print(f"[ERROR] Error handling client: {e}")
                client_socket.close()

    except KeyboardInterrupt:
        print("\n[INFO] Shutting down server...")
        server_socket.close()
        print("[INFO] Server closed")
        sys.exit(0)

if __name__ == '__main__':
    main()
