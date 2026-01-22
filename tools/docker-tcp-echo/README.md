# Docker TCP Echo Server

Standalone Docker module for testing TCP communication.

## Quick Start

```bash
docker-compose up
```

Server will listen on:
- IP: 192.168.1.1 (Docker network)
- Port: 4242
- Protocol: TCP

## What It Does

- Python TCP echo server in Docker container
- Automatically configured with docker-compose
- No manual setup needed - just run!

## Files

- `Dockerfile` - Container definition
- `docker-compose.yml` - Docker Compose configuration
- `tcp_echo_server.py` - Echo server implementation

## Network

The docker-compose.yml creates an isolated Docker network (192.168.1.0/24):
- Server IP: 192.168.1.1
- Port: 4242

## Commands

```bash
# Start server
docker-compose up

# Stop server
Ctrl+C

# View logs
docker-compose logs -f

# Clean up
docker-compose down
```

## Requirements

- Docker Desktop installed

## Usage

1. Start the server from this folder
2. Flash the board with Example 5 firmware
3. Board will connect to 192.168.1.1:4242
4. Watch the echo server log for connections
