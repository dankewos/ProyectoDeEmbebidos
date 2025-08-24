# MazeBot — Vehículo embebido con ESP32-CAM

**Descripción breve.**  
MazeBot es un carrito de laberinto controlado de forma remota desde una app móvil (MIT App Inventor). Transmite video en tiempo real con **ESP32-CAM**, registra estadísticas de la partida (tiempo, giros, puntuación) y permite control por voz. Este repositorio contiene el **firmware** y las **configuraciones específicas de la ESP32-CAM** (pines, cámara, modos Wi-Fi y entorno de compilación con PlatformIO).

---

## Características
- Control de motores DC mediante driver
- Sensores IR/sensor de contacto para eventos de inicio/fin y colisiones.
- App móvil (MIT App Inventor) para control por botones y voz.
- Opción de enviar datos a **Firebase** (tiempos, puntuación, trayectoria).
---

## Configuración ESP32-CAM (incluida en este repo)
- **Modelo de cámara:** AI-Thinker (preconfigurado).
- **GPIO asignados**
  | Señal | GPIO |
  |------|------|
  | IN1  | 13   |
  | IN2  | 15   |
  | IN3  | 4    |
  | IN4  | 14   |
- **Sensor de contacto:** pin **configurable** (por defecto: documentado en el código).
- **Alimentación:** pines **5V** (entrada a regulador/driver), **3.3V** (lógica), y **GND** comunes.
- **Wi-Fi:** modos **AP** y **STA** (credenciales y SSID/AP definidos en el código).
- **Entorno:** `platformio.ini` preparado para **ESP32-CAM** (board `esp32cam`), con monitor serie a 115200 baudios.

---

## Requisitos
- **Visual Studio Code** + extensión **PlatformIO**.
- Cable USB-TTL / programador compatible con ESP32-CAM.
- (Opcional) **Firebase** para registro de datos.
- (Opcional) **Fritzing** (diagramas) y **EasyEDA** (PCB).

---

## Puesta en marcha rápida
```bash
# 1) Clonar
git clone https://github.com/<usuario>/<repo>.git
cd <repo>

# 2) Abrir en VS Code y compilar con PlatformIO
#    PlatformIO: Build → Upload → Monitor Serial (115200)

# 3) Conectar la app móvil
#    - Modo AP: conectarse al SSID configurado
#    - Modo STA: ambos dispositivos en la misma red Wi-Fi
