FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Copy the echo server script
COPY tcp_echo_server.py .

# Expose port 4242
EXPOSE 4242

# Run the echo server
CMD ["python3", "tcp_echo_server.py"]
