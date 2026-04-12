@echo off
REM ============================================
REM OBS AirPlay Receiver - Installer
REM Run as Administrator
REM ============================================

set "PLUGIN_DIR=%PROGRAMDATA%\obs-studio\plugins\obs-airplay-receiver\bin\64bit"
set "SCRIPT_DIR=%~dp0"

echo.
echo OBS AirPlay Receiver - Installer
echo =================================
echo.

REM Create plugin directory
if not exist "%PLUGIN_DIR%" (
    echo Creating directory: %PLUGIN_DIR%
    mkdir "%PLUGIN_DIR%"
    if errorlevel 1 (
        echo.
        echo ERROR: Failed to create directory. Please run as Administrator.
        pause
        exit /b 1
    )
)

REM Copy plugin DLL
echo Copying obs-airplay-receiver.dll...
copy /y "%SCRIPT_DIR%obs-airplay-receiver.dll" "%PLUGIN_DIR%\" >nul
if errorlevel 1 (
    echo ERROR: Failed to copy obs-airplay-receiver.dll
    pause
    exit /b 1
)

REM Copy OpenSSL dependency
echo Copying libcrypto-3-x64.dll...
copy /y "%SCRIPT_DIR%libcrypto-3-x64.dll" "%PLUGIN_DIR%\" >nul
if errorlevel 1 (
    echo ERROR: Failed to copy libcrypto-3-x64.dll
    pause
    exit /b 1
)

echo.
echo ============================================
echo Installation successful!
echo.
echo Plugin installed to:
echo   %PLUGIN_DIR%
echo.
echo Restart OBS Studio, then add a new source:
echo   Sources ^> + ^> AirPlay Receiver
echo ============================================
echo.
pause
