@echo off
set QT_DIR=C:\Qt\6.11.1\mingw_64
set MINGW_DIR=C:\Qt\Tools\mingw1310_64
set PATH=%QT_DIR%\bin;%MINGW_DIR%\bin;%PATH%
cd /d C:\Users\35199\Desktop\SmartParkingQt\build
start SmartParkingQt.exe
