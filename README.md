# 🌡️ Estación Ambiental IoT con Arduino Mega & NodeMCU - Grupo #2

<div align="center">

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![ESP8266](https://img.shields.io/badge/ESP8266-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![IoT](https://img.shields.io/badge/IoT-Project-blue?style=for-the-badge)
![MQTT](https://img.shields.io/badge/MQTT-660066?style=for-the-badge&logo=mqtt&logoColor=white)

**Sistema de monitoreo ambiental en tiempo real con múltiples sensores**

[Características](#-características) • [Arquitectura](#️-arquitectura-del-sistema) • [Instalación](#-instalación) • [Sensores](#-sensores-incluidos) • [Configuración](#️-configuración)

</div>

---

## 📋 Descripción del Proyecto

Este proyecto implementa una **estación meteorológica y de calidad del aire** completa utilizando dos microcontroladores en una arquitectura distribuida:

- **🔵 Arduino Mega 2560**: Actúa como hub central de sensores, recopilando datos de múltiples sensores analógicos y digitales.
- **🔴 NodeMCU ESP8266**: Funciona como gateway WiFi/MQTT, recibiendo datos del Mega y publicándolos en la nube (Ubidots).

### 🎯 Características

✨ **Monitoreo Multi-Sensor**
- 📊 Temperatura y humedad (DHT11)
- 💨 Material particulado PM1.0, PM2.5, PM10 (PMS5003)
- ☠️ Gases peligrosos: CO, O₃, NH₃, NOₓ, CO₂
- 📡 Transmisión inalámbrica a la nube

🚀 **Tecnologías Implementadas**
- Comunicación serial entre microcontroladores
- Protocolo MQTT para IoT
- Procesamiento de datos en tiempo real
- Arquitectura modular y escalable

---

## 🏗️ Arquitectura del Sistema

```
┌─────────────────────────────────────────────────────────────┐
│                    ARDUINO MEGA 2560                        │
│                   (Sensor Hub Master)                       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │  DHT11   │  │ PMS5003  │  │   MQ7    │  │  MQ135   │     │
│  │   Pin4   │  │ Pin12/13 │  │   A0     │  │   A1     │     │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘     │
│                                                             │
│  ┌──────────┐        ┌──────────────────────┐               │
│  │  MQ131   │   -->  │  Procesamiento de    │               │
│  │   A2     │        │  datos y formateo    │               │
│  └──────────┘        └──────────────────────┘               │
│                              ║                              │
│                              ║ Serial1 (TX1/Pin18)          │
│                              ║ 9600 baud                    │
└──────────────────────────────╨──────────────────────────────┘
                               ║
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                   NodeMCU ESP8266                           │
│                  (WiFi/MQTT Gateway)                        │
│                                                             │
│  ┌────────────────┐         ┌────────────────┐              │
│  │ SoftwareSerial │   -->   │  WiFi Module   │              │
│  │  RX: D2/GPIO4  │         │  802.11 b/g/n  │              │
│  └────────────────┘         └────────────────┘              │
│                                     ║                       │
│                                     ║ MQTT                  │
│                                     ║ Port 1883             │
└─────────────────────────────────────╨───────────────────────┘
                                      ║
                                      ▼
                              ☁️ UBIDOTS CLOUD
                           (Visualización & Analytics)
```

### 🔌 Conexiones Físicas

| Arduino Mega | NodeMCU ESP8266 | Descripción |
|:------------:|:---------------:|:-----------:|
| TX1 (Pin 18) | D2 (GPIO4) | Datos seriales |
| GND | GND | Tierra común |
| 5V | VIN | Alimentación (opcional) |

---

## � Estructura del Proyecto

```
estacion_ambiental/
├── README.md                  # Documentación del proyecto
├── arduino_mega/              # Código para Arduino Mega 2560
│   └── sensor_hub.ino         # Firmware del hub de sensores
└── nodemcu_esp8266/           # Código para NodeMCU ESP8266
    └── wifi_gateway.ino       # Firmware del gateway WiFi/MQTT
```

---

## �🚀 Instalación

### 📦 Parte 1: Configuración del NodeMCU (ESP8266)

El Arduino IDE no incluye soporte nativo para ESP8266. Sigue estos pasos:

#### 1️⃣ Añadir el Gestor de URLs Adicionales

1. Abre el Arduino IDE
2. Navega a: **Archivo** → **Preferencias**
3. En el campo **"URLs Adicionales de Gestor de Tarjetas"**, pega:

```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

> 💡 **Tip**: Si ya tienes otras URLs, sepáralas con comas o usa el botón 🗔 junto al campo.

#### 2️⃣ Instalar el Core ESP8266

1. Ve a: **Herramientas** → **Placa** → **Gestor de Tarjetas...**
2. En la barra de búsqueda, escribe: `esp8266`
3. Busca **"esp8266"** de **"ESP8266 Community"**
4. Haz clic en **"Instalar"** y espera a que termine la descarga

#### 3️⃣ Seleccionar la Placa NodeMCU

Una vez instalado:
- Ve a: **Herramientas** → **Placa**
- Selecciona: **"NodeMCU 1.0 (ESP-12E Module)"**

---

### 📚 Parte 2: Instalación de Librerías

Las siguientes librerías son necesarias para el correcto funcionamiento del proyecto.

#### 🔧 Cómo Abrir el Gestor de Librerías

**Programa** → **Incluir Librería** → **Administrar Bibliotecas...**

---

#### 📘 Para el NodeMCU (Gateway WiFi/MQTT)

| Librería | Versión | Autor | Descripción |
|:---------|:--------|:------|:------------|
| **PubSubClient** | Última estable | Nick O'Leary | Cliente MQTT para comunicación con Ubidots |
| **ESP8266WiFi** | ✅ Incluida | ESP8266 Core | Gestión de conexiones WiFi |
| **SoftwareSerial** | ✅ Incluida | ESP8266 Core | Comunicación serial por software |

#### 📗 Para el Arduino Mega (Sensor Hub)

| Librería | Versión | Autor | Descripción |
|:---------|:--------|:------|:------------|
| **DHTStable** | Última estable | Rui Azevedo | Lectura de sensores DHT11/DHT22 |
| **SoftwareSerial** | ✅ Incluida | Arduino AVR | Comunicación serial adicional |
| **avr/pgmspace** | ✅ Incluida | Arduino AVR | Gestión de memoria de programa |

#### 📥 Pasos de Instalación

1. Abre el **Gestor de Librerías** (ver arriba)
2. Busca cada librería por nombre:
   - Escribe: `PubSubClient` → Instalar (Nick O'Leary)
   - Escribe: `DHTStable` → Instalar (Rui Azevedo)
3. Espera a que la instalación termine
4. ¡Listo! Las librerías estarán disponibles para su uso

---

## 🌡️ Sensores Incluidos

### 🌀 DHT11 - Temperatura y Humedad
- **Pin**: 4 (Arduino Mega)
- **Rango Temperatura**: -40°C a +80°C
- **Rango Humedad**: 0% a 100%
- **Tiempo de lectura**: 3 segundos

### 💨 PMS5003 - Material Particulado
- **Pines**: RX→13, TX→12 (Arduino Mega)
- **Mediciones**: PM1.0, PM2.5, PM10
- **Protocolo**: Serial 9600 baud
- **Unidades**: µg/m³

### ☠️ MQ7 - Monóxido de Carbono (CO)
- **Pin**: A0 (Arduino Mega)
- **Rango**: 20-2000 ppm
- **Gas detectado**: CO

### 🌫️ MQ135 - Calidad del Aire
- **Pin**: A1 (Arduino Mega)
- **Gases detectados**: NH₃, NOₓ, CO₂
- **Aplicación**: Monitoreo de calidad del aire interior

### 🧪 MQ131 - Ozono (O₃)
- **Pin**: A2 (Arduino Mega)
- **Rango**: 10-1000 ppb
- **Gas detectado**: Ozono troposférico

---

## ⚙️ Configuración

### 🔐 Credenciales WiFi (NodeMCU)

Edita el archivo `nodemcu_esp8266/wifi_gateway.ino`:

```cpp
#define WIFI_SSID "TU_RED_WIFI"      // 👈 Cambia esto
#define WIFI_PASS "TU_CONTRASEÑA"    // 👈 Cambia esto
```

### 🔑 Token de Ubidots

En el mismo archivo, actualiza tu token:

```cpp
#define UBIDOTS_TOKEN "TU_TOKEN_AQUI"  // 👈 Obtén tu token desde Ubidots
#define DEVICE_LABEL "arduino-estacion-meteorologica"
```

> 📝 **Nota**: Obtén tu token desde [Ubidots](https://ubidots.com/) → Mi Perfil → API Credentials

### ⏱️ Intervalos de Lectura (Arduino Mega)

Puedes ajustar la frecuencia de lectura en `arduino_mega/sensor_hub.ino`:

```cpp
#define SENSOR_INTERVAL 30000   // 30 segundos entre envíos
#define DHT_READ_INTERVAL 3000  // 3 segundos para DHT11
```

---

## 📤 Formato de Datos

### Trama Serial (Mega → NodeMCU)

Formato CSV con 11 campos:

```
T,H,CO,O3,MQ135_ADC,NH3,NOx,CO2,PM1,PM25,PM10
```

**Ejemplo**:
```
25.3,65.2,12.5,8.3,512,15.2,22.8,425.5,10,25,45
```

### Payload MQTT (NodeMCU → Ubidots)

```json
{
  "temperatura": 25.3,
  "humedad": 65.2,
  "co": 12.5,
  "o3": 8.3,
  "mq135_adc": 512,
  "nh3": 15.2,
  "nox": 22.8,
  "co2": 425.5,
  "pm1": 10,
  "pm25": 25,
  "pm10": 45
}
```

---

## 🔍 Monitoreo y Debug

### 💻 Monitor Serial - Arduino Mega

**Puerto**: `/dev/ttyACM0` (Linux) o `COM#` (Windows)
**Baudios**: 9600

Verás logs como:
```
--- ARDUINO MEGA SENSOR HUB (v18.1.0) ---
Enviando datos a NodeMCU por Serial1 (9600 bps)

--- Ciclo de Lectura y Envío ---
Leyendo datos del sensor PMS5003...
Trama enviada: 25.3,65.2,12.5,8.3,512,15.2,22.8,425.5,10,25,45
```

### 📡 Monitor Serial - NodeMCU

**Puerto**: `/dev/ttyUSB0` (Linux) o `COM#` (Windows)
**Baudios**: 9600

Verás logs como:
```
--- NodeMCU Ubidots Gateway v2.0 ---
WiFi Conectado! IP: 192.168.1.105

[RX] Trama CSV recibida del Mega: 25.3,65.2,12.5...
--- DATOS PARSEADOS EN NODEMCU ---
Parseo exitoso (11 campos encontrados).
[TX] Datos MQTT enviados a: /v1.6/devices/arduino-estacion-meteorologica
```

---

## 📊 Versiones del Software

| Componente | Versión | Fecha |
|:-----------|:--------|:------|
| Arduino Mega Firmware | v18.1.0 | Enero 2025 |
| NodeMCU Gateway | v2.0 | Enero 2025 |
| Protocolo de Comunicación | CSV v1.0 | - |

---

## ⚠️ Troubleshooting

### ❌ "WiFi no conecta"
- ✅ Verifica SSID y contraseña
- ✅ Asegúrate de que la red sea 2.4GHz (ESP8266 no soporta 5GHz)
- ✅ Revisa la intensidad de la señal

### ❌ "Error MQTT rc=5"
- ✅ Verifica que el token de Ubidots sea correcto
- ✅ Revisa que el dispositivo exista en tu cuenta

### ❌ "No se reciben datos del Mega"
- ✅ Verifica las conexiones TX1→D2 y GND→GND
- ✅ Asegúrate de que ambos usen 9600 baudios
- ✅ Revisa que el Mega esté enviando datos (Monitor Serial)

### ❌ "Lecturas de sensores erróneas"
- ✅ Verifica las conexiones de alimentación (5V/GND)
- ✅ Revisa que los pines sean correctos
- ✅ Espera al menos 30 segundos para el calentamiento de sensores MQ

---

<div align="center">

**Arquitectura de Computadores y Ensambladores 1 - GRUPO #2**

**⭐ Si este proyecto te fue útil, no olvides darle una estrella ⭐**

Hecho con ❤️ y ☕

</div>
