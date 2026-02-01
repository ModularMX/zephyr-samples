#!/usr/bin/env python3
"""
Simple HTTP server for Zephyr HTTP client testing.
Handles GET and POST requests, logs received data.
"""

import http.server
import socketserver
import json

PORT = 8000

class CustomHTTPHandler(http.server.SimpleHTTPRequestHandler):
    """Custom HTTP handler that supports GET and POST requests."""
    
    def do_GET(self):
        """Handle GET requests."""
        print(f"\n[GET] Request to {self.path}")
        print(f"[GET] Headers: {dict(self.headers)}")
        # Serve directory listing or files
        super().do_GET()
    
    def do_POST(self):
        """Handle POST requests."""
        content_length = int(self.headers.get('Content-Length', 0))
        
        print(f"\n[POST] Request to {self.path}")
        print(f"[POST] Content-Length: {content_length}")
        print(f"[POST] Content-Type: {self.headers.get('Content-Type', 'N/A')}")
        print(f"[POST] Headers: {dict(self.headers)}")
        
        # Read the POST body
        if content_length > 0:
            body = self.rfile.read(content_length)
            print(f"[POST] Raw Body: {body.decode('utf-8', errors='ignore')}")
            
            # Try to parse as JSON
            try:
                data = json.loads(body.decode('utf-8'))
                print(f"[POST] Parsed JSON:")
                for key, value in data.items():
                    print(f"  - {key}: {value}")
            except json.JSONDecodeError:
                print("[POST] Could not parse as JSON")
        
        # Send response
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        
        response = json.dumps({
            "status": "success",
            "message": "POST received by Zephyr server",
            "path": self.path
        })
        self.wfile.write(response.encode('utf-8'))
        print(f"[POST] Response sent: {response}")

Handler = CustomHTTPHandler

try:
    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        print(f"Serving HTTP on 0.0.0.0:{PORT}")
        print(f"Supports GET and POST requests (Ctrl+C to stop)\n")
        httpd.serve_forever()
except KeyboardInterrupt:
    print("\n\n=== Server shutdown ===")
    print("HTTP server stopped gracefully.")
except Exception as e:
    print(f"\n[ERROR] Server error: {e}")
