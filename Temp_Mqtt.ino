#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoMqttClient.h>

#include "mqtt_secrets.h"
#include "wifi_secrets.h"

#define DELAY_TIME 60*1000
// replace with your channel’s thingspeak API key and your SSID and password
String apiKey = THINGSPEAK_APIKEY;
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* server = "api.thingspeak.com";
int connecttimes = 0;

const char mqttServer[] = "mqtt3.thingspeak.com";
int        mqttPort     = 1883;
const char mqttClientID[] = SECRET_MQTT_USERNAME;
const char mqttUser[] = SECRET_MQTT_USERNAME;
const char mqttPass[] = SECRET_MQTT_PASSWORD;
const char mqttTopic[] = "channels/1082634/publish";

// Testeado con:
// - PLACA ESP8266:ESP8266:D1
//
// Opciones de compilacion:
// - Placa: "WeMos D1 R1"
// - Upload Speed: "115200"
// - CPU Frecuency: "80MHz"
// - Debug port: "Serial"
// - Flash Size: "4MB (no spiffs)"
// - lwIp variant: "v2 lower memory"
// - Debug Level: "Ninguno"
// - Exceptions: "Disabled"
// - VTables: "Flash"
// - Erase Flash: "Only sketch"
//
 
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

OneWire  oneWire(2);
DallasTemperature DS18B20(&oneWire);

void setup() 
{
  Serial.begin(115200);
  delay(10);
  
  WiFi.begin(ssid, password);
 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password); 
  connecttimes = 0;
  while (WiFi.status() != WL_CONNECTED && connecttimes < 120 ) 
  {
    delay(500);
    Serial.print(".");
    connecttimes++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot connect to WiFi, resetting...");
    delay(3000);
    WiFi.disconnect();
    ESP.restart(); 
    delay(3000);
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(mqttServer);

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  mqttClient.setId(mqttClientID);

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword(mqttUser, mqttPass);

  if (!mqttClient.connect(mqttServer, mqttPort)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

float getTemp(void)
{
  DS18B20.requestTemperatures();
  return DS18B20.getTempCByIndex(0);
}

void loop() 
{
 
    float tsonda = getTemp();
    String msg = "&field1=" + String(tsonda) + "&status=MQTTPUBLISH";
    mqttClient.poll();

    Serial.print("Sending message to topic: ");
    Serial.println(mqttTopic);
    Serial.println(msg);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(mqttTopic);
    mqttClient.print(msg);
    mqttClient.endMessage();

    Serial.print(" Temp Sonda: ");
    Serial.print(tsonda);
    Serial.println(" C°\nWaiting 1 min ...");
    delay(DELAY_TIME);
}
