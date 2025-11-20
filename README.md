# ESP32 Sensor Device

Proyecto ESP32 desarrollado con PlatformIO.  
Este dispositivo se conecta vía WiFi y publica datos por MQTT hacia un backend NestJS.

## 🚀 Características

- ESP32-WROOM o ESP32-S3
- Plataforma PlatformIO (C++ / Arduino Framework)
- Conexión WiFi 2.4 GHz
- Envío de datos por MQTT
- Estructura modular (src, include, lib)
- Archivo `secrets.h` para credenciales (no versionado)
- Script `.bat` para build + upload + monitor

---

## 📁 Estructura del proyecto

## Primera vez
- Recuerda instalar drivers **drivers CP210x_Windows_Drivers
- Extensiones VSCODE : PlatformIO IDE
- configurar plataforminio.ini
- revisar upload.bat
    - 

## Crear Proyecto
- mkdir nombre_del_proyecto
- cd nombre_del_proyecto

- crear proyecto
- pio project init --board esp32dev