#ifndef __MODBUS_TCP_SERVER_H
#define __MODBUS_TCP_SERVER_H
#include <Arduino.h>
#include "Config.h"
#include "TimeSpan.h"
#include "ModbusCommon.h"

#define MAX_WIFI_CONNECTIONS_COUNT (uint8_t)3

class ModbusTCPServer
{
  public:
    InfoRegisters* infoRegisters = NULL;
    CoilRegisters* coilRegisters = NULL;
    ContactRegisters* contactRegisters = NULL;
    HoldingRegisters* holdingRegisters = NULL;
    InputRegisters* inputRegisters = NULL;
    
    ModbusTCPServer(){}
    ~ModbusTCPServer(){}

    void begin(MemoryManager* memoryManager,PinsStateManager* pinsStateManager)
    { 
      this->memoryManager=memoryManager;
      this->pinsStateManager = pinsStateManager;

      server = WiFiServer(atoi(memoryManager->networkProperty.modbusTCPPort));
      server.begin();
    }

    void communicationErrorProcess(uint8_t connectionNumber)//WiFiClient* wifiClient)
    {
      currentBytes[connectionNumber]=0;
      wifiClients[connectionNumber].flush();
      wifiClients[connectionNumber].stop();
      connectionStages[connectionNumber] = MODBUS_TCP_CONNECTION_STARTED;//MODBUS_TCP_CONNECTION_CLOSED;
    }

    void inputDataProcessing(uint8_t connectionNumber)//WiFiClient* wifiClien)
    {
      while(wifiClients[connectionNumber].available())
      {
        modbusTCPBuffers[connectionNumber][currentBytes[connectionNumber]++]=wifiClients[connectionNumber].read();
        if(currentBytes[connectionNumber]==MODBUS_TCP_BUFFER_SIZE)
        {
          communicationErrorProcess(connectionNumber);          
          break;
        }
      }

      if(currentBytes[connectionNumber]>7)//check header
      {
        ModbusTCPHeader* modbusTCPHeader = (ModbusTCPHeader*)modbusTCPBuffers[connectionNumber];//communicationErrorProcess()
        uint16_t dataLength = bigToLittle16(modbusTCPHeader->dataLength);//modbusFrameTCP->lenght - big endian
        if(dataLength<currentBytes[connectionNumber]-6)return;
        if(modbusTCPHeader->protocolID!=0x0000)
        {
          communicationErrorProcess(connectionNumber);
          return;
        }

        switch(modbusTCPHeader->functionCode)
        {
          case BROADCAST_REQUEST:break;//do nothing
          case READ_COIL_REG :
          {
            if(coilRegisters==NULL)
            {
              modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
              modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
              dataLength=3;//dev address, function code, error
              modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
            }
            else
            {
              uint16_t addressStartReg = (modbusTCPHeader->data[0]<<8)|modbusTCPHeader->data[1];
              uint16_t amountOfRegs = (modbusTCPHeader->data[2]<<8)|modbusTCPHeader->data[3];
        
              if(coilRegisters->registerCount<addressStartReg+amountOfRegs)
              {
                modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
                modbusTCPHeader->data[0]=ILLEGAL_DATA_ADDRESS;
                dataLength=3;//dev address, function code, error
                modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
              }
              if(amountOfRegs%8)
              { 
                uint8_t answerByteCount = amountOfRegs/8 + 1;        
                modbusTCPHeader->data[0]=answerByteCount;
                uint16_t allDataLenght = 3+answerByteCount;  
                volatile int i=0;       
                for(i=0;i<answerByteCount;i++)
                {
                  modbusTCPHeader->data[1+i]=((coilRegisters->dataBytes[addressStartReg/8+i])>>addressStartReg%8)|((coilRegisters->dataBytes[addressStartReg/8+i+1])<<(8-addressStartReg%8));
                }
                uint8_t amountOfnull = answerByteCount*8 - amountOfRegs;
                uint8_t mask=0xFF>> amountOfnull;
                modbusTCPHeader->data[i]&=mask;
                modbusTCPHeader->dataLength = littleToBig16(allDataLenght);
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],allDataLenght+6);//+6 - transactionID, protocolID,dataLength//return allDataLenght;
              }
              else
              {
                uint8_t answerByteCount = amountOfRegs/8;        
                modbusTCPHeader->data[0]=answerByteCount;
                uint16_t allDataLenght = 3+answerByteCount;  
                volatile int i=0;
                if(addressStartReg%8)//не ровно
                {           
                  for(i=0;i<answerByteCount;i++)
                  {
                    modbusTCPHeader->data[1+i]=((coilRegisters->dataBytes[addressStartReg/8+i])>>addressStartReg%8)|((coilRegisters->dataBytes[addressStartReg/8+i+1])<<(8-addressStartReg%8));
                  }
                }
                else
                {
                  for(i=0;i<answerByteCount;i++)
                  {
                    modbusTCPHeader->data[1+i]=coilRegisters->dataBytes[addressStartReg/8+i];
                  }
                }
                modbusTCPHeader->dataLength = littleToBig16(allDataLenght);
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],allDataLenght+6);//+6 - transactionID, protocolID,dataLength//return allDataLenght;
              } 
            }
          }
          break;
          case READ_CONTACT_REG :
          {
            if(contactRegisters==NULL)
            {
              modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
              modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
              dataLength=3;//dev address, function code, error
              modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
            }
            else
            {
              uint16_t addressStartReg = (modbusTCPHeader->data[0]<<8)|modbusTCPHeader->data[1];
              uint16_t amountOfRegs = (modbusTCPHeader->data[2]<<8)|modbusTCPHeader->data[3];
        
              if(contactRegisters->registerCount<addressStartReg+amountOfRegs)
              {
                modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
                modbusTCPHeader->data[0]=ILLEGAL_DATA_ADDRESS;
                dataLength=3;//dev address, function code, error
                modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
              }
              if(amountOfRegs%8)
              { 
                uint8_t answerByteCount = amountOfRegs/8 + 1;        
                modbusTCPHeader->data[0]=answerByteCount;
                uint16_t allDataLenght = 3+answerByteCount;  
                volatile int i=0;       
                for(i=0;i<answerByteCount;i++)
                {
                  modbusTCPHeader->data[1+i]=((contactRegisters->dataBytes[addressStartReg/8+i])>>addressStartReg%8)|((contactRegisters->dataBytes[addressStartReg/8+i+1])<<(8-addressStartReg%8));
                }
                uint8_t amountOfnull = answerByteCount*8 - amountOfRegs;
                uint8_t mask=0xFF>> amountOfnull;
                modbusTCPHeader->data[i]&=mask;
                modbusTCPHeader->dataLength = littleToBig16(allDataLenght);
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],allDataLenght+6);//+6 - transactionID, protocolID,dataLength//return allDataLenght;
              }
              else
              {
                uint8_t answerByteCount = amountOfRegs/8;        
                modbusTCPHeader->data[0]=answerByteCount;
                uint16_t allDataLenght = 3+answerByteCount;  
                volatile int i=0;
                if(addressStartReg%8)//не ровно
                {           
                  for(i=0;i<answerByteCount;i++)
                  {
                    modbusTCPHeader->data[1+i]=((contactRegisters->dataBytes[addressStartReg/8+i])>>addressStartReg%8)|((contactRegisters->dataBytes[addressStartReg/8+i+1])<<(8-addressStartReg%8));
                  }
                }
                else
                {
                  for(i=0;i<answerByteCount;i++)
                  {
                    modbusTCPHeader->data[1+i]=contactRegisters->dataBytes[addressStartReg/8+i];
                  }
                }
                modbusTCPHeader->dataLength = littleToBig16(allDataLenght);
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],allDataLenght+6);//+6 - transactionID, protocolID,dataLength//return allDataLenght;
              } 
            }
          }
          break;
          case READ_HOLDING_REG :
            if(holdingRegisters==NULL)
            {
              modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
              modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
              dataLength=3;//dev address, function code, error
              modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
            }
            else
            {
              uint16_t addressStartReg = (modbusTCPHeader->data[0]<<8)|modbusTCPHeader->data[1];
              uint16_t amountOfRegs = (modbusTCPHeader->data[2]<<8)|modbusTCPHeader->data[3];
        
              if(holdingRegisters->registerCount<addressStartReg+amountOfRegs)
              {
                modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
                modbusTCPHeader->data[0]=ILLEGAL_DATA_ADDRESS;
                dataLength=3;//dev address, function code, error
                modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
              }
              else
              {
                uint8_t amountOfBytesForAnswer = amountOfRegs*2;        
                modbusTCPHeader->data[0]=amountOfBytesForAnswer;
                uint16_t allDataLenght = 3+amountOfBytesForAnswer;  
                volatile int i=0;
                for(i=0;i<amountOfRegs;i++)
                {         
                  modbusTCPHeader->data[1+i*2]=holdingRegisters->registers[addressStartReg+i]&0xFF;//Версия анны//>>8;
                  modbusTCPHeader->data[2+i*2]=holdingRegisters->registers[addressStartReg+i]>>8;//Версия Анны//&0xFF;
                }
                //if(packetReceiveCallback!=NULL)server->packetReceiveCallback(READ_HOLDING_REG,addressStartReg,amountOfRegs,&server->holdingRegisters->HoldingRegisters[addressStartReg]);
                modbusTCPHeader->dataLength = littleToBig16(allDataLenght);
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],allDataLenght+6);//+6 - transactionID, protocolID,dataLength//return allDataLenght;
              } 
            }
          break;
          case READ_INPUT_REG :
            if(inputRegisters==NULL)
            {
              modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
              modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
              dataLength=3;//dev address, function code, error
              modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
            }
            else
            {
              uint16_t addressStartReg = (modbusTCPHeader->data[0]<<8)|modbusTCPHeader->data[1];
              uint16_t amountOfRegs = (modbusTCPHeader->data[2]<<8)|modbusTCPHeader->data[3];
        
              if(inputRegisters->registerCount<addressStartReg+amountOfRegs)
              {
                modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
                modbusTCPHeader->data[0]=ILLEGAL_DATA_ADDRESS;
                dataLength=3;//dev address, function code, error
                modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
              }
              else
              {
                uint8_t amountOfBytesForAnswer = amountOfRegs*2;        
                modbusTCPHeader->data[0]=amountOfBytesForAnswer;
                uint16_t allDataLenght = 3+amountOfBytesForAnswer;  
                volatile int i=0;
                for(i=0;i<amountOfRegs;i++)
                {         
                  modbusTCPHeader->data[1+i*2]=inputRegisters->registers[addressStartReg+i]&0xFF;//Версия анны//>>8;
                  modbusTCPHeader->data[2+i*2]=inputRegisters->registers[addressStartReg+i]>>8;//Версия Анны//&0xFF;
                }

                //if(packetReceiveCallback!=NULL)server->packetReceiveCallback(READ_HOLDING_REG,addressStartReg,amountOfRegs,&server->holdingRegisters->HoldingRegisters[addressStartReg]);
                modbusTCPHeader->dataLength = littleToBig16(allDataLenght);
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],allDataLenght+6);//+6 - transactionID, protocolID,dataLength//return allDataLenght;
              } 
            }
          break;
          case WRITE_COIL_REGISTER :
            if(coilRegisters==NULL)
            {
              modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
              modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
              dataLength=3;//dev address, function code, error
              modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
            }
            else
            {
              uint16_t addressReg = (modbusTCPHeader->data[0]<<8)|modbusTCPHeader->data[1];
        
              if(coilRegisters->registerCount<=addressReg)
              {
                modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
                modbusTCPHeader->data[0]=ILLEGAL_DATA_ADDRESS;
                dataLength=3;//dev address, function code, error
                modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength 
              }
        
              uint16_t writeValue = (modbusTCPHeader->data[2]<<8)|modbusTCPHeader->data[3];
        
              uint8_t registerByte = addressReg/8;
              uint8_t registerShift = addressReg%8;
              uint8_t mask = 1<<registerShift;
        
              if(writeValue==0xFF00)coilRegisters->dataBytes[registerByte]=coilRegisters->dataBytes[registerByte]|mask;
              else coilRegisters->dataBytes[registerByte]=coilRegisters->dataBytes[registerByte]&~(uint8_t)mask;
              /*if(packetReceiveCallback!=NULL)packetReceiveCallback(WRITE_COIL_REGISTER,addressReg,1,(uint16_t*)server->coilRegisters->CoilRegisterBytes);*/
              //delete
              //if((memoryManager->doutStateFormulaStr[0]=='0'&&memoryManager->doutStateFormulaStr[1]==0)||(memoryManager->doutStateFormulaStr[0]=='1'&&memoryManager->doutStateFormulaStr[1]==0))pinsStateManager->setDOUT1State(coilRegisters->dataBytes[0]&0x01);
              
              dataLength=6;
              modbusTCPHeader->dataLength = littleToBig16(dataLength);
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength//return 6;
            } 
          break;
          case WRITE_HOLDING_REGISTER :
            modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
            modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
            dataLength=3;//dev address, function code, error
            modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
            if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
          break;
          case WRITE_COIL_REGISTERS :
            if(coilRegisters==NULL)
            {
              modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
              modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
              dataLength=3;//dev address, function code, error
              modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
            }
            else
            {
              uint16_t addressStartReg = (modbusTCPHeader->data[0]<<8)|modbusTCPHeader->data[1];
              uint16_t amountOfRegs = (modbusTCPHeader->data[2]<<8)|modbusTCPHeader->data[3];
        
              if(coilRegisters->registerCount<addressStartReg+amountOfRegs)
              {
                modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
                modbusTCPHeader->data[0]=ILLEGAL_DATA_ADDRESS;
                dataLength=3;//dev address, function code, error
                modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength 
              }
              //uint8_t amountOfBytesForWrite=frameRTU->data[4];
              volatile int i=0;
              uint8_t registerByte;
              uint8_t registerShift;
              uint8_t mask;
              for(i=0;i<amountOfRegs;i++)
              {
                registerByte = (addressStartReg+i)/8;
                registerShift = (addressStartReg+i)%8;
                mask = 1<<registerShift;
                if(modbusTCPHeader->data[5+(i/8)]&(1<<(i%8)))coilRegisters->dataBytes[registerByte]=coilRegisters->dataBytes[registerByte]|mask;
                else coilRegisters->dataBytes[registerByte]=coilRegisters->dataBytes[registerByte]&~(uint8_t)mask;
              }
              
              dataLength=6;
              modbusTCPHeader->dataLength = littleToBig16(dataLength);
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength//return 6;
            } 
          break;
          case WRITE_HOLDING_REGISTERS :
            if(holdingRegisters==NULL)
            {
              modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
              modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
              dataLength=3;//dev address, function code, error
              modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
            }
            else
            {
              uint16_t addressStartReg = (modbusTCPHeader->data[0]<<8)|modbusTCPHeader->data[1];
              uint16_t amountOfRegs = (modbusTCPHeader->data[2]<<8)|modbusTCPHeader->data[3];
        
              if(holdingRegisters->registerCount<addressStartReg+amountOfRegs)
              {
                modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
                modbusTCPHeader->data[0]=ILLEGAL_DATA_ADDRESS;
                dataLength=3;//dev address, function code, error
                modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
                if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength  
              }
              //uint8_t amountOfBytesForWrite=frameRTU->data[4];
              volatile int i;        
              for(i=0;i<amountOfRegs;i++)
              {         
                holdingRegisters->registers[addressStartReg+i]=modbusTCPHeader->data[5+i*2]|modbusTCPHeader->data[6+i*2]<<8;//frameTCP->data[6+i*2]|frameTCP->data[5+i*2]<<8;
              }
              //if(packetReceiveCallback!=NULL)packetReceiveCallback(WRITE_HOLDING_REGISTERS,addressStartReg,amountOfRegs,&server->holdingRegisters->HoldingRegisters[addressStartReg]); 
              dataLength=6;
              modbusTCPHeader->dataLength = littleToBig16(dataLength); 
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength//return 6;
            }
          break;
          case READ_INFO_ABOUT_DEVICE ://(Serial Line only)
            if(infoRegisters==NULL)
            {
              modbusTCPHeader->functionCode=modbusTCPHeader->functionCode|0x80;
              modbusTCPHeader->data[0]=ILLEGAL_FUNCTION;
              dataLength=3;//dev address, function code, error
              modbusTCPHeader->dataLength = littleToBig16(dataLength);//modbusTCPHeader->lenght - big endian
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength
            }
            else
            {
              modbusTCPHeader->data[0]=infoRegisters->dataLength;
              volatile int i;
              for(i=0;i<infoRegisters->dataLength;i++)
              {
                modbusTCPHeader->data[1+i]=infoRegisters->data[i];
              }
              dataLength=3+infoRegisters->dataLength;
              modbusTCPHeader->dataLength = littleToBig16(dataLength);
              if(wifiClients[connectionNumber].connected())wifiClients[connectionNumber].write(modbusTCPBuffers[connectionNumber],dataLength+6);//+6 - transactionID, protocolID,dataLength//vreturn 3+server->infoRegisters->dataLength;
            }
          break;
        }    
        currentBytes[connectionNumber]=0;
      }
    }

    void loop()
    {
      for(int i=0;i<MAX_WIFI_CONNECTIONS_COUNT;i++)
      {
        if(connectionStages[i] == MODBUS_TCP_CONNECTION_STARTED)
        {
          wifiClients[i] = server.available();
          if (wifiClients[i])
          {
            #ifdef SERIAL_DEBUG_ENABLED
              Serial.print("connection started: ");
              Serial.println(i,DEC);
            #endif
            connectionStages[i] = MODBUS_TCP_CONNECTION_ONGOING;
            currentBytes[i]=0;
            inputDataProcessing(i);//&wifiClients[i]);
          }
        }
      }
      for(int i=0;i<MAX_WIFI_CONNECTIONS_COUNT;i++)
      {
        if(connectionStages[i] == MODBUS_TCP_CONNECTION_ONGOING)
        {
          if(!wifiClients[i].connected())
          {
            #ifdef SERIAL_DEBUG_ENABLED
              Serial.print("connection finished: ");
              Serial.println(i,DEC);
            #endif
            communicationErrorProcess(i);
            break;
          }

          inputDataProcessing(i);
        }
      }
    }
    
  private:
    WiFiServer server = WiFiServer(502);
    WiFiClient wifiClients[MAX_WIFI_CONNECTIONS_COUNT];
    ModbusTCPConnectionStage connectionStages[MAX_WIFI_CONNECTIONS_COUNT]={MODBUS_TCP_CONNECTION_STARTED,MODBUS_TCP_CONNECTION_STARTED,MODBUS_TCP_CONNECTION_STARTED};
    
    MemoryManager* memoryManager;
    PinsStateManager* pinsStateManager; 
    uint8_t modbusTCPBuffers[MAX_WIFI_CONNECTIONS_COUNT][MODBUS_TCP_BUFFER_SIZE];
    uint16_t currentBytes[MAX_WIFI_CONNECTIONS_COUNT]={0,0,0};
};







#endif
