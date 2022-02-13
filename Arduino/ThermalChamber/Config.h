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

struct DataModel{
  float Temperature;//float A_IN_Voltage;
  float Humidity;
  uint8_t D_OUT_1_Control;
};

DataModel dataModel={
  .Temperature=-273.0F,//.A_IN_Voltage=3.1F,
  .Humidity=0,
  .D_OUT_1_Control=0x01,
};

InputRegisters inputRegisters = {2,(uint16_t*)&dataModel.Temperature};
CoilRegisters coilRegisters = {1,&dataModel.D_OUT_1_Control};

const uint8_t infoString[]="R_04_LOW_COST";
InfoRegisters infoRegisters={sizeof(infoString),(uint8_t*)infoString};

#endif
