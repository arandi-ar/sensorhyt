#define __ESP32_MQTT_H__

// Defino el pin del sensor DHT 11
#define DHT11PIN 16

#include "DHT.h"
#include "config.h"

// Inlcuyo  las librerias para conectarme a internet
#include <Client.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>


// Incluyo las librerias mqtt y google cloud
#include "MQTT.h"

#include "CloudIoTCore.h"
#include "CloudIoTCoreMqtt.h"

DHT dht(DHT11PIN, DHT11);

// Inicializo Wifi y MQTT
Client *netClient;
CloudIoTCoreDevice *device;
CloudIoTCoreMqtt *mqtt;
MQTTClient *mqttClient;
unsigned long iat = 0;
String jwt;


void setup() {
  Serial.begin(115200);
  dht.begin();
  device = new CloudIoTCoreDevice(
      project_id, location, registry_id, device_id,
      private_key_str);

  setupWifi();
  netClient = new WiFiClientSecure();
  mqttClient = new MQTTClient(512);
  mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
  mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);
  mqtt->setUseLts(true);
  mqtt->startMQTT();
}

unsigned long lastMillis = 0;
void loop() {
  mqtt->loop();
  delay(100);  // <- fixes some issues with WiFi stability

  if (!mqttClient->connected()) {
    connect();
  }

  if (millis() - lastMillis > 60000) {
    lastMillis = millis();
    float humedad = dht.readHumidity();
    float temperatura = dht.readTemperature();
  
//    Serial.println(String(temperatura) + ", " + String(humedad));
     mqtt->publishTelemetry(String(temperatura)+", "+String(humedad)+"");
  }
}


void setupWifi() {
  Serial.println("Inicializando wifi");

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  Serial.println("Conectando al WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  configTime(0, 0, ntp_primary, ntp_secondary);
  Serial.println("Esperando on time sync...");
  while (time(nullptr) < 1510644967) {
    delay(10);
  }
}

void connectWifi() {
  Serial.print("Chequeando wifi wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
}

String getJwt() {
  iat = time(nullptr);
  Serial.println("Refrescando JWT");
  jwt = device->createJWT(iat, jwt_exp_secs);
  return jwt;
}

void connect() {
  connectWifi();
  mqtt->mqttConnect();
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Recibiendo: " + topic + " - " + payload);
}
