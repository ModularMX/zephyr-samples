@echo off
setlocal enabledelayedexpansion
REM Real-time packet capture display (simple format)

set TSHARK_PATH=C:\Program Files\Wireshark\tshark.exe

if not exist "!TSHARK_PATH!" (
    echo [ERROR] tshark not found
    pause
    exit /b 1
)

echo Starting real-time packet capture on port 4443...
echo Filter: tcp.port == 4443
echo Press Ctrl+C to stop
echo.

REM Display packets in simple one-line format
"!TSHARK_PATH!" -i Ethernet -f "tcp port 4443" -Y "tcp.port == 4443 or tls"

echo.
echo [OK] Capture complete
pause
