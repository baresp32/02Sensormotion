@echo off
title ESP32 - Build, Upload & Monitor
echo ============================================
echo   ESP32 - Compilar, Subir y Monitorear
echo ============================================
echo.

REM --- Paso 1: Compilar ---
echo [1/3] Compilando el proyecto...
pio run
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ Error en la compilacion. Revisa el codigo.
    pause
    exit /b %ERRORLEVEL%
)

REM --- Paso 2: Subir al ESP32 ---
echo.
echo [2/3] Subiendo al ESP32...
pio run --target upload
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ Error durante la subida. ¿Puerto correcto? ¿ESP32 conectado?
    pause
    exit /b %ERRORLEVEL%
)

REM --- Paso 3: Abrir el monitor serial ---
echo.
echo [3/3] Abriendo el monitor serial en 115200 baud...
echo (Presiona CTRL + C para salir)
pio device monitor -b 115200

echo.
pause
