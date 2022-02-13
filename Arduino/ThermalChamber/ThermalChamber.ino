#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "Config.h"
#include "MemoryManager.h"
#include "TimeSpan.h"
#include "PinsStateManager.h"
#include "WIFIManager.h"
#include "DHT.h"

#define DHTTYPE DHT11
// DHT датчик
uint8_t DHTPin = D1;/*GPIO5*/; 
               
// Инициализация датчика DHT
DHT dht(DHTPin, DHTTYPE);       

WIFIManager wifiManager;
PinsStateManager pinsStateManager;

MemoryManager memoryManager;
Ticker pinsStateManagerLoopTicker;

void setup()
{
  pinMode(DHTPin, INPUT);
  dht.begin();  
  
  digitalWrite(LED_PIN, HIGH);
  pinMode(LED_PIN, OUTPUT);
  
  #ifdef SERIAL_DEBUG_ENABLED
  Serial.begin(115200);
  Serial.println();
  #endif

  memoryManager.begin();
  pinsStateManager.begin(&memoryManager);
  
  dataModel.Temperature = dht.readTemperature(); // Получает значения температуры
  dataModel.Humidity = dht.readHumidity(); // Получает значения влажности

  pinsStateManagerLoopTicker.attach(1.0,[](){pinsStateManager.loop();}); 

  wifiManager.begin(&memoryManager,&pinsStateManager);
}

void loop() {
  dataModel.Temperature = dht.readTemperature(); // Получает значения температуры
  dataModel.Humidity = dht.readHumidity(); // Получает значения влажности
  
  wifiManager.loop();
}
