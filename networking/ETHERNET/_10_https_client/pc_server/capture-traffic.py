#!/usr/bin/env python3
"""
Simple packet capture for HTTPS debugging
Requires: pip install scapy
"""

import sys
try:
    from scapy.all import sniff, wrpcap
except ImportError:
    print("ERROR: scapy not installed")
    print("Install with: pip install scapy")
    sys.exit(1)

DEVICE_IP = "192.168.1.100"
SERVER_IP = "192.168.1.1"
OUTPUT_FILE = "https_capture.pcap"

def packet_callback(packet):
    """Print packet summary"""
    print(f"[PACKET] {packet.summary()}")

print(f"Starting packet capture...")
print(f"Device: {DEVICE_IP}")
print(f"Server: {SERVER_IP}")
print(f"Output: {OUTPUT_FILE}")
print("Press Ctrl+C to stop\n")

try:
    # Capture packets between device and server
    packets = sniff(
        filter=f"host {DEVICE_IP}",
        prn=packet_callback,
        store=True
    )
except KeyboardInterrupt:
    print(f"\n\nSaving {len(packets)} packets to {OUTPUT_FILE}...")
    wrpcap(OUTPUT_FILE, packets)
    print(f"Done! Open with: wireshark {OUTPUT_FILE}")
