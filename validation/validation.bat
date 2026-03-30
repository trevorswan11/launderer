@echo off
setlocal

set LOG_DATE=%date:~12,2%-%date:~4,2%-%date:~7,2%
set FILENAME=validation-%LOG_DATE%.csv

echo Starting ESP-IDF Monitor...
echo Logging to: %FILENAME%
echo Press Ctrl+C to stop.

idf.py monitor 1>> %FILENAME% 2>nul
pause
