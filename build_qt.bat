@echo off
echo ========================================
echo   SmartParking Qt Build Script
echo ========================================
echo.

set QT_DIR=C:\Qt\6.11.1\mingw_64
set PATH=%QT_DIR%\bin;C:\Qt\Tools\mingw1310_64\bin;%PATH%

echo [1/3] Configuring with CMake...
cmake -B build_qt -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%QT_DIR%
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [2/3] Building...
cmake --build build_qt -j%NUMBER_OF_PROCESSORS%
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo [3/3] Build successful!
echo.
echo To run: build_qt\SmartParkingQt.exe
echo.
pause
