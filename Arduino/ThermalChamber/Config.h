#ifndef __CONFIG_H
#define __CONFIG_H
#include "ModbusCommon.h"

#define SERIAL_DEBUG_ENABLED

#define LED_PIN           2
#define SELECT_MODE_PIN   4
#define T_H_SENSOR_PIN    5/*D1*/
#define OUT1_HIGH_PIN     12
#define OUT1_LOW_PIN      13
#define OUT2_HIGH_PIN     14
#define OUT2_LOW_PIN      15

#define A_IN              A0  /* ESP8266 Analog Pin ADC0 = A0*/

struct ModbusData{
float Temperature;
float Humidity;
uint16_t doutState;
uint16_t controlAllowed;
};

ModbusData modbusData ={
  .Temperature=0,
  .Humidity=0,
  .doutState=1,/*0 - heating, 1 - disabled, 2 - coolling*/
  .controlAllowed=1,/*0- denied, 1 -allowed*/
};

InputRegisters inputRegisters = {5,(uint16_t*)&modbusData.Temperature};
CoilRegisters coilRegisters = {1,(uint8_t*)&modbusData.controlAllowed};

const uint8_t infoString[]="R_04_LOW_COST";
InfoRegisters infoRegisters={sizeof(infoString),(uint8_t*)infoString};

#endif
