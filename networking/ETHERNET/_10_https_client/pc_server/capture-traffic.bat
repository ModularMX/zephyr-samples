@echo off
setlocal enabledelayedexpansion
REM Wireshark capture script for HTTPS debugging

echo Starting packet capture...
echo Press Ctrl+C to stop
echo.

set TSHARK_PATH=C:\Program Files\Wireshark\tshark.exe

if not exist "!TSHARK_PATH!" (
    echo [ERROR] tshark not found at !TSHARK_PATH!
    pause
    exit /b 1
)

echo [OK] Found tshark
echo Capturing traffic on port 4443
echo Saving to: https_capture.pcap
echo.

REM Run tshark to capture on all interfaces, filter by port 4443
"!TSHARK_PATH!" -f "tcp port 4443" -w https_capture.pcap

echo.
echo [OK] Capture saved to https_capture.pcap
echo.
echo To analyze:
echo   1. Open file: C:\Program Files\Wireshark\wireshark.exe https_capture.pcap
echo   2. Filter: tcp.port == 4443
echo.
pause



