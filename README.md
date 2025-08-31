# MazeBot: Vehículo Embebido de Navegación Remota para Resolución de Laberinto
## Proyecto de Embebidos – Sistema Móvil (Carrito)
**Autores:**  
- Aceldo Torres María Grazia | maactorr@espol.edu.ec  
- Romero Lambogglia Christopher Ariel | chrarome@espol.edu.ec  

## I. INTRODUCCIÓN
Este documento presenta el diseño y desarrollo del sistema móvil MazeBot, un robot diferencial controlado remotamente mediante una aplicación móvil. Su propósito es brindar una experiencia lúdica y educativa, integrando tecnologías embebidas con **ESP32-CAM**, transmisión de video en tiempo real y control mediante interfaz accesible en MIT App Inventor.  
El usuario guía al MazeBot a través de un laberinto físico (https://github.com/dankewos/ProyectoDeEmbebidos2.git), mientras el sistema transmite video y registra métricas de desempeño. Se busca estimular habilidades cognitivas relacionadas con la orientación espacial, la toma de decisiones y la planificación estratégica. La inclusión se promueve mediante **control por voz** para usuarios con movilidad reducida.

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
- **Wi-Fi:** modos **STA** (credenciales y SSID/AP definidos en el código).
- **Entorno:** `platformio.ini` preparado para **ESP32-CAM** (board `esp32cam`), con monitor serie a 115200 baudios.

---
## Requisitos
- **Visual Studio Code** + extensión **PlatformIO**.
- Cable USB-TTL / programador compatible con ESP32-CAM.
- **Firebase** para registro de datos.
-  **EasyEDA** (PCB).

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
```

## II. ALCANCE Y LIMITACIONES
### A. Alcance
- **Sistema MazeBot (Hardware y Electrónica):**: El proyecto comprende un robot móvil diferencial diseñado para desplazarse dentro de un laberinto controlado, permitiendo ejecutar tareas de exploración, recorrido y resolución de trayectorias. Su funcionalidad principal es ofrecer una experiencia lúdico-educativa en la que los usuarios puedan interactuar con el entorno, considerando accesibilidad mediante controles simples e inclusión al permitir su uso por distintos perfiles de estudiantes. Incluye un robot móvil diferencial con ESP32-CAM, motores DC, baterías 18650 con módulo BMS y un regulador de voltaje. Todo se integra en una PCB compacta, diseñada para facilitar ensamblaje y operación eficiente. El MazeBot opera en una maqueta de laberinto que constituye su entorno de prueba.  
- **Interfaz de Usuario:** (Aplicación móvil y control): La aplicación, desarrollada en MIT App Inventor, permite control remoto mediante botones direccionales o comandos de voz. Integra transmisión de video en tiempo real, configuración del juego y visualización de estadísticas personalizadas.  
- **Detección y Retroalimentación:** Emplea sensores IR en la entrada y salida del laberinto para detectar inicio y fin de recorrido. Se registran métricas como tiempo total, cantidad de giros, puntuación por stickers y se genera una gráfica de la trayectoria, ofreciendo retroalimentación visual y cuantitativa.
- **Orientación Educativa e Inclusiva:** El sistema está orientado principalmente a alumnos de colegio como herramienta para aprender y practicar actividades cognitivas relacionadas con la planificación, la memoria y la resolución de problemas. No obstante, cualquier persona puede utilizarlo con fines recreativos o de entrenamiento cognitivo. El control por voz constituye una función inclusiva que amplía la accesibilidad del sistema.
---

### B. Limitaciones
- Respecto a funcionalidad: El MazeBot depende completamente del control manual; no integra navegación autónoma ni algoritmos de resolución de laberintos. Tampoco implementa visión computacional avanzada para identificación de objetos de interés. Además, depende bastante de la conexión a la red puesto que funciona con WiFi, pero no como AccessPoint.
- Entorno: El sistema está diseñado exclusivamente para interiores, sin protección contra polvo, humedad o interferencias externas, por lo que su uso está limitado a ambientes hostiles.
- Almacenamiento: Actualmente no incluye integración en la nube ni almacenamiento permanente, aunque se contempla como posible mejora futura para análisis remoto o historial de partidas.

---

## III. DIAGRAMAS EXPLICATIVOS
### A. Diagrama de bloques
Representa la arquitectura electrónica del MazeBot:  
- ESP32-CAM como microcontrolador central.  
- Sistema de alimentación: 2 baterías 18650 en serie (7.4V, 4200mAh), módulo BMS y regulador de voltaje.  
- Driver L298N para control de motores DC.  
- Sensores de contacto para detección de colisiones.
![Diagrama 1](img/diagrama1.png)

### B. Máquina de estados del carrito
Control de arranque, movimiento y retroalimentación de estados del robot mediante transiciones lógicas.

---

## IV. ALTERNATIVAS DE DISEÑO
- **Microcontrolador:** ESP32-CAM seleccionado por costo, eficiencia y simplicidad frente a Arduino UNO + WiFi y Raspberry Pi Zero W.  
- **Alimentación:** 2x 18650 en serie, elegidas sobre PowerBank o LiPo por seguridad y autonomía.  
- **Comunicación:** Modo **STA**, con proyección a AP/STA para redundancia y configuración inicial.  
- **Control de motores:** L298N elegido por disponibilidad y capacidad (2A por canal), con control PWM a 1kHz.  

---

## V. PLAN DE TEST Y VALIDACIÓN
- **Pruebas funcionales:** Control remoto desde app, transmisión de video, detección de sensores IR y registro de métricas.  
- **Métricas:** Latencia de video, estabilidad Wi-Fi, detección de eventos, consumo energético.  
- **Escenarios:** Recorridos completos en maqueta, detección de stickers, repetición de pruebas en condiciones variadas.  

---

## VI. CONSIDERACIONES TÉCNICAS
- Inclusión mediante **control por voz**.  
- Privacidad con futura integración de cifrado AES y autenticación.  
- Seguridad eléctrica con BMS y reguladores.  
- Fines educativos, sin monetización.  
- Potencial evolución hacia **visión computacional** y **navegación autónoma**.  

---

## VII. REFERENCIAS
[1] D. Zhu et al., *Advanced Drug Delivery Reviews*, vol. 138, 2019.  
[2] G. Galeano, *Programación de sistemas embebidos en C*, Alfaomega, 2011.  
[3] E. J. Chancay Sancán Quimis, Tesis de Ingeniería, UNESUM, 2024.  
[4] P. F. Salazar, Trabajo de grado, UNAD, 2024.  

