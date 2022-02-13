#ifndef __MODBUS_COMMON_H
#define __MODBUS_COMMON_H
#include <Arduino.h>

//-----Mdbus Functions------//
#define BROADCAST_REQUEST        (uint8_t)0x00
#define READ_COIL_REG            (uint8_t)0x01
#define READ_CONTACT_REG         (uint8_t)0x02
#define READ_HOLDING_REG         (uint8_t)0x03
#define READ_INPUT_REG           (uint8_t)0x04
#define WRITE_COIL_REGISTER      (uint8_t)0x05
#define WRITE_HOLDING_REGISTER   (uint8_t)0x06
#define READ_STATUS              (uint8_t)0x07/*прочитать статус прибора (8 бит)(Serial Line only)*/
#define TEST_FUNCTION            (uint8_t)0x08/*Тестовая функция(Serial Line only)*/
#define WRITE_COIL_REGISTERS     (uint8_t)0x0F
#define WRITE_HOLDING_REGISTERS  (uint8_t)0x10
#define READ_INFO_ABOUT_DEVICE   (uint8_t)0x11/*(Serial Line only)*/
#define READ_FILE_RECORD         (uint8_t)0x14/*(Serial Line only)*/
#define WRITE_FILE_RECORD        (uint8_t)0x15
#define READ_FIFO                (uint8_t)0x18/*чтение очереди данных*/

#define WRITE_COEFICIENTS        (uint8_t)0x65/*пользовательская функция*/

//-----End Mdbus Functions------//

//-----Exception code------------//
#define ILLEGAL_FUNCTION          (uint8_t)0x01/*Недопустимый код функции. Этот код исключения может возвращаться, если устройство не реализует поддержку некоторых функций Modbus протокола.*/
#define ILLEGAL_DATA_ADDRESS      (uint8_t)0x02/*Недопустимый адрес регистра. Этот код исключения может возвращаться, если запрос указывает для чтения несуществующие регистры.*/
#define ILLEGAL_DATA_VALUE        (uint8_t)0x03/*Недопустимое значение данных. Этот код исключения может возвращаться, если запрос содержит недопустимое значение для записи в регистр.*/
#define SLAVE_DEVICE_FAILURE      (uint8_t)0x04/*Во время выполнения запрошенного действия произошла ошибка и действие не может быть завершено.*/
#define ACKNOWLAGE                (uint8_t)0x05/*Специальный ответ, который используется совместно с командами программирования. Означает, что ведомое устройство приняло запрос и обрабатывает его, но данная операция потребует продолжительное время для завершения. Такой ответ возвращается, что бы предотвратить таймаут на мастере.*/
#define SLAVE_DEVICE_BUSY         (uint8_t)0x06/*Специальный ответ, который используется совместно с командами программирования. Означает, что устройство занято обработкой длинного запроса.*/
#define NEGATIVE_ACKNOWLAGE       (uint8_t)0x07/*Специальный ответ, который используется совместно с командами программирования. Означает, что длинная операция закончилась с ошибкой.*/
#define MEMORY_PARITY_ERROR       (uint8_t)0x08/*Специальный ответ, который используется совместно с командами 20 и 21. Эти команды можно найти в официальной документации протокола Modbus.*/
#define GATEWAY_PATH_UNAVAILABLE  (uint8_t)0x10/*Этот ответ означает, что устройство выступает в роли gateway (портала) и не может переслать запрос с входного порта на запрошенное устройство. Обычно значит, что запрос не может быть переслан, так как канал коммуникации отсутствует (сеть не подключена, порт не инициализирован и т.д.).*/
#define GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND  (uint8_t)0x11/*Этот ответ означает, что устройство выступает в роли gateway (портала) и не может получить ответ от запрошенного устройства. Т.е. запрос был переслан на порт устройства. Но gateway не получил от него ответ.*/
//------End exceptio code---------//

#define bigToLittle16(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define littleToBig16(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define bigToLittle32(a) ((((a)>>24)&0x000000ff)|(((a)<<8)&0x00ff0000)|(((a)>>8)&0x0000ff00)|(((a)<<24)&0xff000000))

#define MODBUS_TCP_BUFFER_SIZE (uint16_t)512

struct ModbusTCPHeader{
  uint16_t transactionID;
  uint16_t protocolID;/*always 0x0000*/
  uint16_t dataLength;
  uint8_t devAddress;
  uint8_t functionCode;
  uint8_t data[];
};

enum ModbusTCPConnectionStage{
  MODBUS_TCP_CONNECTION_STARTED,
  MODBUS_TCP_CONNECTION_ONGOING,
  /*MODBUS_TCP_CONNECTION_CLOSED,*/
};

struct ModbusRTUHeader{
  uint8_t devAddress;
  uint8_t functionCode;
  uint8_t data[];
};

struct InfoRegisters{
  uint8_t dataLength;
  uint8_t * data;
};

struct CoilRegisters{    //чтение / запись
  uint16_t registerCount;
  uint8_t *dataBytes;
};

struct ContactRegisters{ //чтение
  uint16_t registerCount;
  uint8_t *dataBytes;
};

struct HoldingRegisters{ //чтение / запись
  uint16_t registerCount;
  uint16_t *registers;
};

struct InputRegisters{   //чтение
  uint16_t registerCount;
  uint16_t *registers;
};



#endif
