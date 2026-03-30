@echo off
setlocal

set LOG_DATE=%date:~12,2%-%date:~4,2%-%date:~7,2%
set HR=%time:~0,2%
if "%HR:~0,1%"==" " set HR=0%HR:~1,1%
set LOG_TIME=%HR%-%time:~3,2%
set FILENAME=validation-%LOG_DATE%-%LOG_TIME%.csv

echo Starting ESP-IDF Monitor...
echo Logging to: %FILENAME%
echo Press Ctrl+] to stop.
idf.py monitor 1>> %FILENAME% 2>nul

pause