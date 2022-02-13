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

      /*if((state_D_IN_1 = digitalRead(D_IN_1)))dataModel.D_IN_OUT_States|=0x01;
      else dataModel.D_IN_OUT_States&=~0x01;

      if((state_D_IN_2 = digitalRead(D_IN_2)))dataModel.D_IN_OUT_States|=0x02;
      else dataModel.D_IN_OUT_States&=~0x02;*/
      
      /*state_D_IN_1 = digitalRead(D_IN_1);
      state_D_IN_2 = digitalRead(D_IN_2);*/
      DINModeState = digitalRead(SELECT_MODE_PIN);

      /*if(this->memoryManager->doutStateFormulaStr[0]=='1')digitalWrite(D_OUT_1, HIGH);
      else digitalWrite(D_OUT_1, LOW);*/

      /*if((state_D_OUT_1 = digitalRead(D_OUT_1)))dataModel.D_IN_OUT_States|=0x04;
      else dataModel.D_IN_OUT_States&=~0x04;*/
      
      //state_D_OUT_1 = digitalRead(D_OUT_1);
    }

    void loop()
    {  
      //dataModel.A_IN_Voltage=(state_A_IN = (analogRead(A_IN)*3.3F/1024.0F)*this->memoryManager->ainCoefficent);//state_A_IN = (analogRead(A_IN)*3.3F/1024.0F)*this->memoryManager->ainCoefficent;
      
      /*if((state_D_IN_1 = digitalRead(D_IN_1)))dataModel.D_IN_OUT_States|=0x01;
      else dataModel.D_IN_OUT_States&=~0x01;

      if((state_D_IN_2 = digitalRead(D_IN_2)))dataModel.D_IN_OUT_States|=0x02;
      else dataModel.D_IN_OUT_States&=~0x02;*/
      /*state_D_IN_1 = digitalRead(D_IN_1);
      state_D_IN_2 = digitalRead(D_IN_2);*/
      /*if(!((memoryManager->doutStateFormulaStr[0]=='0'||memoryManager->doutStateFormulaStr[0]=='1')&&memoryManager->doutStateFormulaStr[1]==0))
      {
        bool result = booleanExpressonResult(3
                                          ,this->memoryManager->doutStateFormulaStr
                                          ,"A_IN",state_A_IN
                                          ,"D_IN_1",state_D_IN_1
                                          ,"D_IN_2",state_D_IN_2);
        if(result)digitalWrite(D_OUT_1, HIGH);
        else digitalWrite(D_OUT_1, LOW);
      }*/

      /*if((state_D_OUT_1 = digitalRead(D_OUT_1)))dataModel.D_IN_OUT_States|=0x04;
      else dataModel.D_IN_OUT_States&=~0x04;*/
      /*state_D_OUT_1 = digitalRead(D_OUT_1);*/
    }

    //float getAINState(){return state_A_IN;}
    /*uint8_t getDIN1State(){return state_D_IN_1;}
    uint8_t getDIN2State(){return state_D_IN_2;}*/
    uint8_t getDINModeState(){return DINModeState;}
    /*void setDOUT1State(uint8_t state)
    {
      if(state)digitalWrite(D_OUT_1, HIGH);
      else digitalWrite(D_OUT_1, LOW);
    }*/
    uint8_t getDOUTState(){return DOUTState;/*return digitalRead(D_OUT_1);*/}
  private:
    
    //float state_A_IN = 0;
    /*uint8_t state_D_IN_1 =0;
    uint8_t state_D_IN_2 =0;*/
    uint8_t DINModeState =0;
    uint8_t DOUTState =0;
    MemoryManager* memoryManager;
};


#endif
