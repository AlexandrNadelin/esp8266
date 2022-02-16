#ifndef __PINS_STATE_MANAGER_H
#define __PINS_STATE_MANAGER_H
#include <Arduino.h>
#include "Config.h"
#include "MathParser.h"
#include "MemoryManager.h"
#include "StringReplaser.h"

class PinsStateManager
{
  public:
    PinsStateManager(){}
    ~PinsStateManager(){}
    void begin(MemoryManager* memoryManager)
    {
      this->memoryManager=memoryManager;
      pinMode(SELECT_MODE_PIN, INPUT_PULLUP);
      
      digitalWrite(OUT1_HIGH_PIN, LOW);
      pinMode(OUT1_HIGH_PIN, OUTPUT);
      
      digitalWrite(OUT1_LOW_PIN, LOW);
      pinMode(OUT1_LOW_PIN, OUTPUT);
      
      digitalWrite(OUT2_HIGH_PIN, LOW);
      pinMode(OUT2_HIGH_PIN, OUTPUT);
      
      digitalWrite(OUT2_LOW_PIN, LOW);
      pinMode(OUT2_LOW_PIN, OUTPUT);

      //dataModel.A_IN_Voltage=(state_A_IN = (analogRead(A_IN)*3.3F/1024.0F)*this->memoryManager->ainCoefficent);

      DINModeState = digitalRead(SELECT_MODE_PIN);
    }

    void loop()
    {  
      if(modbusData.controlAllowed==0)return;

      if(modbusData.Temperature<memoryManager->tMin && modbusData.doutState!=0)//Температура меньше минимума и нагреватель не включен
      {//Включаем нагреватель
        digitalWrite(OUT2_LOW_PIN, LOW);
        digitalWrite(OUT1_HIGH_PIN, LOW);
        delayMicroseconds(1000);
        digitalWrite(OUT1_LOW_PIN, HIGH);
        digitalWrite(OUT2_HIGH_PIN, HIGH);
        modbusData.doutState=0;
      }
      else if(modbusData.Temperature>memoryManager->tMax && modbusData.doutState!=2)//Температура больше максимума и охладитель не включен
      {//Включаем охладитель
        digitalWrite(OUT1_LOW_PIN, LOW);
        digitalWrite(OUT2_HIGH_PIN, LOW);
        delayMicroseconds(1000);
        digitalWrite(OUT2_LOW_PIN, HIGH);
        digitalWrite(OUT1_HIGH_PIN, HIGH);
        modbusData.doutState=2;
      }
      else if(modbusData.Temperature>=memoryManager->tMin && modbusData.Temperature<=memoryManager->tMax && modbusData.doutState!=1)//Температура в порядке
      {//Выключаем все
        digitalWrite(OUT1_LOW_PIN, LOW);
        digitalWrite(OUT1_HIGH_PIN, LOW);
        delayMicroseconds(1000);
        digitalWrite(OUT2_HIGH_PIN, LOW);
        digitalWrite(OUT2_LOW_PIN, LOW);
        modbusData.doutState=1;
      }
      
      //A_IN_Voltage=(state_A_IN = (analogRead(A_IN)*3.3F/1024.0F)*this->memoryManager->ainCoefficent);//state_A_IN = (analogRead(A_IN)*3.3F/1024.0F)*this->memoryManager->ainCoefficent;
    }

    uint8_t DINModeState =0;    
  private:    
    MemoryManager* memoryManager;
};


#endif
