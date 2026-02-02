#!/usr/bin/python

# HTTPS server test application for IPv4
# Listens on 0.0.0.0:4443 (all interfaces, IPv4)

import socket
from http.server import HTTPServer
from http.server import SimpleHTTPRequestHandler
import ssl

PORT = 4443

class RequestHandler(SimpleHTTPRequestHandler):
    length = 0
    protocol_version = 'HTTP/1.1'  # Enable persistent connections

    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-Type', 'text/html')
        self.send_header('Content-Length', str(self.length))
        self.send_header('Connection', 'close')  # Close after each request
        self.end_headers()

    def do_GET(self):
        """Handle GET request"""
        payload = b"<html><p>GET response from HTTPS Server</p></html>"
        self.length = len(payload)
        self._set_headers()
        self.wfile.write(payload)
        self.wfile.flush()

    def do_POST(self):
        """Handle POST request"""
        # Read POST data
        content_length = self.headers.get('Content-Length')
        if content_length:
            post_data = self.rfile.read(int(content_length))
            print(f"[POST] Received {len(post_data)} bytes: {post_data[:100]}")
        
        payload = b"<html><p>POST response from HTTPS Server - Done</p></html>"
        self.length = len(payload)
        self._set_headers()
        self.wfile.write(payload)
        self.wfile.flush()

    def log_message(self, format, *args):
        """Log incoming requests"""
        print(f"[{self.client_address[0]}:{self.client_address[1]}] {format % args}")

def main():
    # Bind to all IPv4 interfaces
    httpd = HTTPServer(("0.0.0.0", PORT), RequestHandler)
    print(f"Starting HTTPS Server on 0.0.0.0:{PORT}")
    print(f"Certificate: ./https-server.pem")
    
    # Create SSL context compatible with mbedTLS/Zephyr
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.check_hostname = False
    context.verify_mode = ssl.CERT_NONE
    
    # Set minimum TLS version to 1.2 (compatible with Zephyr)
    context.minimum_version = ssl.TLSVersion.TLSv1_2
    context.maximum_version = ssl.TLSVersion.TLSv1_2
    
    # Enable all cipher suites including older ones for compatibility
    context.set_ciphers('ECDHE-RSA-AES128-SHA256:ECDHE-RSA-AES256-SHA384:DHE-RSA-AES128-SHA256:DHE-RSA-AES256-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ALL:@SECLEVEL=0')
    
    context.load_cert_chain(certfile='./https-server.pem')
    httpd.socket = context.wrap_socket(httpd.socket, server_side=True)
    
    print(f"[OK] Server listening on all interfaces (IPv4)")
    print(f"[OK] TLS 1.2 enabled with compatible cipher suites")
    print(f"Connect to: https://192.168.1.1:{PORT}")
    print(f"Press Ctrl+C to stop\n")
    
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n[INFO] Server stopped")
        httpd.server_close()

if __name__ == '__main__':
    main()
