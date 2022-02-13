#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "Config.h"
#include "MemoryManager.h"
#include "TimeSpan.h"
#include "PinsStateManager.h"
#include "WIFIManager.h"
#include "DHT.h"
               
// Инициализация датчика DHT
DHT dht(T_H_SENSOR_PIN, DHT11);       

WIFIManager wifiManager;
PinsStateManager pinsStateManager;

MemoryManager memoryManager;

void setup()
{
  pinMode(T_H_SENSOR_PIN, INPUT);
  dht.begin();  
  
  digitalWrite(LED_PIN, HIGH);
  pinMode(LED_PIN, OUTPUT);
  
  #ifdef SERIAL_DEBUG_ENABLED
  Serial.begin(115200);
  Serial.println();
  #endif

  memoryManager.begin();
  pinsStateManager.begin(&memoryManager);

  wifiManager.begin(&memoryManager,&pinsStateManager);
}

void loop() {
  modbusData.Temperature = dht.readTemperature(); // Получает значения температуры
  modbusData.Humidity = dht.readHumidity(); // Получает значения влажности

  pinsStateManager.loop();
  
  wifiManager.loop();
}
