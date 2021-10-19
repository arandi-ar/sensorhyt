#include "DHT.h"
#define DHT11PIN 16

DHT dht(DHT11PIN, DHT11);

void setup() {
  Serial.begin(115200);
  dht.begin();

}

void loop() {
  delay(6000);
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();

  Serial.println(String(temperatura) + ", " + String(humedad));}
