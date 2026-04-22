/*
 * ESTE CÓDIGO DEBE SER FLASHEADO EN EL NODEMCU ESP8266.
 * El NodeMCU actuará como gateway: recibirá los datos del Arduino Mega
 * por su puerto Serial y los enviará a Ubidots.
 *
 * VERSION v2.1 (MQ136 integrado)
 *
 * CONEXIONES:
 * Mega TX1 (Pin 18) -> NodeMCU D2 (GPIO4)
 * Mega GND          -> NodeMCU GND
 *
 * IMPORTANTE:
 * En Arduino IDE selecciona:
 * Herramientas > Placa > NodeMCU 1.0 (ESP-12E Module)
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

// --- Configuración WiFi y Ubidots ---
#define WIFI_SSID "TU_RED_WIFI"          // Cambia por tu red WiFi
#define WIFI_PASS "TU_CONTRASEÑA_WIFI"   // Cambia por tu contraseña WiFi
#define UBIDOTS_TOKEN "TU_TOKEN_UBIDOTS" // Obtén tu token desde Ubidots
#define DEVICE_LABEL "estacion-meto"
#define MQTT_CLIENT_NAME "nodemcu-gateway"

// --- Pines para SoftwareSerial ---
// D2 en NodeMCU equivale a GPIO4
#define MEGA_RX_PIN D2

// RX, TX (-1 = no usado)
SoftwareSerial megaSerial(MEGA_RX_PIN, -1);

// --- Objetos ---
WiFiClient espClient;
PubSubClient client(espClient);

// --- Tiempos ---
unsigned long lastConnectAttempt = 0;
const long connectInterval = 10000;

// --- Trama de datos recibida del Mega ---
// Formato esperado:
// T,H,CO,O3,MQ135_ADC,NH3,NOx,CO2,MQ136_ADC,SO2,PM1,PM25,PM10
struct SensorData
{
    float T = 0;
    float H = 0;
    float CO = 0;
    float O3 = 0;
    int MQ135_ADC = 0;
    float NH3 = 0;
    float NOx = 0;
    float CO2 = 0;
    int MQ136_ADC = 0;
    float SO2 = 0;
    int PM1 = 0;
    int PM25 = 0;
    int PM10 = 0;
};

SensorData data;

// --- Prototipos ---
void connectToWiFi();
void connectToUbidots();
bool parseAndSaveData(const char *dataStr);
void publishDataToUbidots(const SensorData &d);
void printParsedData(const SensorData &d);

void setup()
{
    // Serial del NodeMCU para debug en la PC
    Serial.begin(9600);

    // Serial por software para escuchar al Mega
    megaSerial.begin(9600);

    Serial.println(F("\n--- NodeMCU Ubidots Gateway v2.1 (MQ136 integrado) ---"));
    Serial.println(F("DEBUG por Serial."));
    Serial.println(F("Recepción del Mega por GPIO4 (D2) a 9600 baudios."));
    Serial.println(F("CONEXION: Mega TX1(Pin18) -> NodeMCU D2(GPIO4), GND->GND"));

    client.setServer("industrial.api.ubidots.com", 1883);

    connectToWiFi();
}

void loop()
{
    // Mantener conexión MQTT
    if (!client.connected())
    {
        if (millis() - lastConnectAttempt > connectInterval)
        {
            connectToUbidots();
            lastConnectAttempt = millis();
        }
    }

    client.loop();

    // Leer trama del Arduino Mega
    if (megaSerial.available())
    {
        String dataStr = megaSerial.readStringUntil('\n');
        dataStr.trim();

        Serial.print(F("\n[RX] Trama CSV recibida del Mega: "));
        Serial.println(dataStr);

        if (dataStr.length() > 0 && parseAndSaveData(dataStr.c_str()))
        {
            printParsedData(data);

            if (client.connected())
            {
                publishDataToUbidots(data);
            }
            else
            {
                Serial.println(F("Ubidots no conectado. Intentando reconectar..."));
                connectToUbidots();
            }
        }
        else if (dataStr.length() > 0)
        {
            Serial.println(F("ERROR: Fallo al parsear la trama de datos."));
        }
    }
}

// --- FUNCIONES DE CONECTIVIDAD ---

void connectToWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print(F("Conectando a WiFi"));

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(F("."));
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print(F("WiFi conectado. IP: "));
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println();
        Serial.println(F("ERROR: No se pudo conectar a WiFi. Verifica SSID y contraseña."));
    }
}

void connectToUbidots()
{
    Serial.print(F("Conectando a Ubidots (MQTT)..."));

    if (client.connect(MQTT_CLIENT_NAME, UBIDOTS_TOKEN, UBIDOTS_TOKEN))
    {
        Serial.println(F(" ¡Conectado!"));
    }
    else
    {
        Serial.print(F(" Falló, rc="));
        Serial.print(client.state());
        Serial.println(F(". Reintentando..."));
    }
}

// --- PARSEO ---

bool parseAndSaveData(const char *dataStr)
{
    int result = sscanf(
        dataStr,
        "%f,%f,%f,%f,%d,%f,%f,%f,%d,%f,%d,%d,%d",
        &data.T,
        &data.H,
        &data.CO,
        &data.O3,
        &data.MQ135_ADC,
        &data.NH3,
        &data.NOx,
        &data.CO2,
        &data.MQ136_ADC,
        &data.SO2,
        &data.PM1,
        &data.PM25,
        &data.PM10
    );

    return (result == 13);
}

// --- DEBUG ---

void printParsedData(const SensorData &d)
{
    Serial.println(F("--- DATOS PARSEADOS EN NODEMCU ---"));
    Serial.println(F("Parseo exitoso (13 campos encontrados)."));

    Serial.print(F("Temperatura: "));
    Serial.print(d.T);
    Serial.print(F(" | Humedad: "));
    Serial.print(d.H);
    Serial.print(F(" | CO: "));
    Serial.println(d.CO);

    Serial.print(F("O3: "));
    Serial.print(d.O3);
    Serial.print(F(" | MQ135 ADC: "));
    Serial.print(d.MQ135_ADC);
    Serial.print(F(" | NH3: "));
    Serial.println(d.NH3);

    Serial.print(F("NOx: "));
    Serial.print(d.NOx);
    Serial.print(F(" | CO2: "));
    Serial.print(d.CO2);
    Serial.print(F(" | MQ136 ADC: "));
    Serial.print(d.MQ136_ADC);
    Serial.print(F(" | SO2: "));
    Serial.println(d.SO2);

    Serial.print(F("PM1/PM2.5/PM10: "));
    Serial.print(d.PM1);
    Serial.print(F("/"));
    Serial.print(d.PM25);
    Serial.print(F("/"));
    Serial.println(d.PM10);

    Serial.println(F("----------------------------------"));
}

// --- PUBLICACIÓN MQTT ---

void publishDataToUbidots(const SensorData &d)
{
    if (!client.connected())
    {
        Serial.println(F("ERROR: Cliente MQTT desconectado."));
        return;
    }

    char payload[400];

    sprintf(
        payload,
        "{\"temperatura\":%.1f,\"humedad\":%.1f,\"co\":%.1f,\"o3\":%.1f,\"mq135_adc\":%d,\"nh3\":%.1f,\"nox\":%.1f,\"co2\":%.1f,\"mq136_adc\":%d,\"so2\":%.1f,\"pm1\":%d,\"pm25\":%d,\"pm10\":%d}",
        d.T,
        d.H,
        d.CO,
        d.O3,
        d.MQ135_ADC,
        d.NH3,
        d.NOx,
        d.CO2,
        d.MQ136_ADC,
        d.SO2,
        d.PM1,
        d.PM25,
        d.PM10
    );

    char topic[80];
    sprintf(topic, "/v1.6/devices/%s", DEVICE_LABEL);

    if (client.publish(topic, payload))
    {
        Serial.print(F("[TX] Datos MQTT enviados a: "));
        Serial.println(topic);
        Serial.print(F("[TX] Payload: "));
        Serial.println(payload);
    }
    else
    {
        Serial.println(F("ERROR: Fallo en la publicación MQTT."));
    }
}