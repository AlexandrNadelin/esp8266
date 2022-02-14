#ifndef __HTTP_SERVER_H
#define __HTTP_SERVER_H
#include <Arduino.h>
#include "Config.h"
#include <ESP8266WiFi.h>
#include "MemoryManager.h"
#include "TimeSpan.h"
#include "PinsStateManager.h"
#include "WebSite.h"

/*enum HTTPServerMode{
  STA_MODE,
  AP_MODE,
};*/

const static char PAGE_HEADER[] = 
"HTTP/1.1 200 OK\r\n"
"Server: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\n"
"Content-type: %s\r\n"
"Content-Length: %d\r\n"
"Connection: close\r\n\r\n";/*keep-alive\r\n\r\n";*//*text/html*/

#define RECEIVE_ARRAY_SIZE (uint16_t)2000
char receiveArray[RECEIVE_ARRAY_SIZE];
uint32_t receiveDataCounter=0;

class HTTPServer
{
  public:
    HTTPServer(){}
    ~HTTPServer(){}

    void begin(MemoryManager* memoryManager,PinsStateManager* pinsStateManager)
    {      
      this->memoryManager=memoryManager;
      this->pinsStateManager = pinsStateManager;
            
      server.begin();

      #ifdef SERIAL_DEBUG_ENABLED
      Serial.println("HTTP server started");
      Serial.println();
      #endif
    }
    
    void loop()
    {
      WiFiClient client = server.available();
      // wait for a client (web browser) to connect
      if (client)
      {
        //Serial.println("Client connected");
        receiveDataCounter=0;
        previousActionTimeSt=millis();
        while(client.connected())
        {
          if(client.available())
          {
            previousActionTimeSt=millis();
            receiveArray[receiveDataCounter++]=client.read();
            if(receiveDataCounter==RECEIVE_ARRAY_SIZE)
            {
              Serial.println("RECEIVE_ARRAY_SIZE exceeded");
              break;
            }  
            else if(receiveDataCounter>=4&&memcmp(&receiveArray[receiveDataCounter-4],"\r\n\r\n",4)==0)
            {
              if(memcmp(&receiveArray[0],"GET / HTTP/1.1",sizeof("GET / HTTP/1.1")-1)==0||memcmp(&receiveArray[0],"GET /Index.html",sizeof("GET /Index.html")-1)==0)
              {   
                int len = sprintf(receiveArray,PAGE_HEADER,"text/html",size_Index_html);
                client.write(receiveArray,len);
                client.write(data_Index_html,size_Index_html);
              }              
              /*else if(memcmp(&receiveArray[0],"GET /favicon.ico",sizeof("GET /favicon.ico")-1)==0)
              {
                int len = sprintf(receiveArray,PAGE_HEADER,"image/x-icon",size_favicon_ico);
                client.write(receiveArray,len);
                //client.write(data_favicon_ico,size_favicon_ico);
                
                int i=0;
                for(;i<size_favicon_ico/1000;i++)
                {
                  client.write(&data_favicon_ico[i*1000],1000);
                }
                client.write(&data_favicon_ico[i*1000],size_favicon_ico-i*1000);
              }*/
              else if(memcmp(&receiveArray[0],"GET /style.css",sizeof("GET /style.css")-1)==0)
              {
                int len = sprintf(receiveArray,PAGE_HEADER,"text/css",size_style_css);
                client.write(receiveArray,len);
                client.write(data_style_css,size_style_css);
              }
              else if(memcmp(&receiveArray[0],"GET /TemperatureControl.html",sizeof("GET /TemperatureControl.html")-1)==0)
              {
                int len = sprintf(receiveArray,PAGE_HEADER,"text/html",size_TemperatureControl_html);
                client.write(receiveArray,len);
                client.write(data_TemperatureControl_html,size_TemperatureControl_html);
              }
              else if(memcmp(&receiveArray[0],"GET /networkParameters.property",sizeof("GET /networkParameters.property")-1)==0)
              {
                //---reading property---//
                File networkParameters;
                if(!(networkParameters = SPIFFS.open("/networkParameters.property", "r")))
                {
                  #ifdef SERIAL_DEBUG_ENABLED
                  Serial.println("Failed to open networkParameters.property to read");
                  #endif
                  break;
                }

                MemoryManager memoryManager;

                int numberOfBytes = sprintf(receiveArray,"{\"ApSSID\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"ApPASSWORD\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"ApIPAddress\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"ApSubnetMask\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"ApGateway\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"ModbusTCPPort\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
        
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"StSSID\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"StPASSWORD\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"StIPAddress\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"StSubnetMask\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"StGateway\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);
                /*numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"MQTTPublishPeriod\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&networkParameters,&receiveArray[numberOfBytes]);*/
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\"}");
                receiveArray[numberOfBytes]=0;
 
                networkParameters.close();                

                int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",numberOfBytes);//at the end

                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);
                client.write(receiveArray,numberOfBytes);
              }
              else if(memcmp(&receiveArray[0],"GET /reboot",sizeof("GET /reboot")-1)==0)
              {
                int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",0);//at the end
                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);
                delay(400);
                ESP.restart();                
              }
              else if(memcmp(&receiveArray[0],"POST /networkParameters.property",sizeof("POST /networkParameters.property")-1)==0)
              {
                int dataLen=0;
                for(int i=sizeof("POST /networkParameters.property HTTP/1.1")-1;i<receiveDataCounter;i++)
                {
                  if(memcmp(&receiveArray[i],"Content-Length:",sizeof("Content-Length:")-1)==0)
                  {
                    dataLen = strtol(&receiveArray[i+sizeof("Content-Length:")-1], NULL, 10);
                  }
                }

                int dataCount=0;
                while(dataCount<dataLen)
                {
                  while(client.available())
                  {
                    previousActionTimeSt = millis();
                    receiveArray[dataCount]=client.read();
                    dataCount++;
                  }
                  
                  currentTimeSt=millis();
                  if(calculateTimeSpan(currentTimeSt, previousActionTimeSt)>1000)
                  {                    
                    #ifdef SERIAL_DEBUG_ENABLED
                    Serial.println("Receive timeout 1000ms");
                    #endif
                    break;
                  }
                }
                if(dataCount<dataLen)break;

                File networkParameters;
                if(!(networkParameters = SPIFFS.open("/networkParameters.property", "w")))
                {
                  #ifdef SERIAL_DEBUG_ENABLED
                  Serial.println("Failed to open networkParameters.property to write");
                  #endif
                  break;
                }  

                for(dataCount = 2; dataCount < dataLen-(sizeof("ApSSID")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"ApSSID",sizeof("ApSSID")-1)==0)
                  {
                    dataCount+=(sizeof("ApSSID")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("ApPASSWORD")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"ApPASSWORD",sizeof("ApPASSWORD")-1)==0)
                  {
                    dataCount+=(sizeof("ApPASSWORD")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("ApIPAddress")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"ApIPAddress",sizeof("ApIPAddress")-1)==0)
                  {
                    dataCount+=(sizeof("ApIPAddress")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("ApSubnetMask")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"ApSubnetMask",sizeof("ApSubnetMask")-1)==0)
                  {
                    dataCount+=(sizeof("ApSubnetMask")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("ApGateway")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"ApGateway",sizeof("ApGateway")-1)==0)
                  {
                    dataCount+=(sizeof("ApGateway")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("ModbusTCPPort")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"ModbusTCPPort",sizeof("ModbusTCPPort")-1)==0)
                  {
                    dataCount+=(sizeof("ModbusTCPPort")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("StSSID")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"StSSID",sizeof("StSSID")-1)==0)
                  {
                    dataCount+=(sizeof("StSSID")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("StPASSWORD")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"StPASSWORD",sizeof("StPASSWORD")-1)==0)
                  {
                    dataCount+=(sizeof("StPASSWORD")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("StIPAddress")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"StIPAddress",sizeof("StIPAddress")-1)==0)
                  {
                    dataCount+=(sizeof("StIPAddress")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("StSubnetMask")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"StSubnetMask",sizeof("StSubnetMask")-1)==0)
                  {
                    dataCount+=(sizeof("StSubnetMask")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("StGateway")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"StGateway",sizeof("StGateway")-1)==0)
                  {
                    dataCount+=(sizeof("StGateway")-1+3);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      networkParameters.print(receiveArray[dataCount++]);
                    }
                    networkParameters.println();
                    break;
                  }
                }

                networkParameters.close();

                int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",0);//at the end
                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);            
              }
              else if(memcmp(&receiveArray[0],"POST /setTMinTMax",sizeof("POST /setTMinTMax")-1)==0)
              {
                int dataLen=0;
                for(int i=sizeof("POST /setTMinTMax HTTP/1.1")-1;i<receiveDataCounter;i++)
                {
                  if(memcmp(&receiveArray[i],"Content-Length:",sizeof("Content-Length:")-1)==0)
                  {
                    dataLen = strtol(&receiveArray[i+sizeof("Content-Length:")-1], NULL, 10);
                  }
                }

                int dataCount=0;
                while(dataCount<dataLen)
                {
                  while(client.available())
                  {
                    previousActionTimeSt = millis();
                    receiveArray[dataCount]=client.read();
                    dataCount++;
                  }
                  
                  currentTimeSt=millis();
                  if(calculateTimeSpan(currentTimeSt, previousActionTimeSt)>1000)
                  {                    
                    #ifdef SERIAL_DEBUG_ENABLED
                    Serial.println("Receive timeout 1000ms");
                    #endif
                    break;
                  }
                }
                if(dataCount<dataLen)break;

                for(dataCount = 2; dataCount < dataLen-(sizeof("tMin")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"tMin",sizeof("tMin")-1)==0)
                  {
                    dataCount+=(sizeof("tMin")-1+3);
                    memoryManager->tMin=atof(&receiveArray[dataCount]);
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("tMax")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"tMax",sizeof("tMax")-1)==0)
                  {
                    dataCount+=(sizeof("tMin")-1+3);
                    memoryManager->tMax=atof(&receiveArray[dataCount]);
                    break;
                  }
                }

                int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",0);//at the end
                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);            
              }
              else if(memcmp(&receiveArray[0],"POST /saveTMinTMax",sizeof("POST /saveTMinTMax")-1)==0)
              {
                int dataLen=0;
                for(int i=sizeof("POST /saveTMinTMax HTTP/1.1")-1;i<receiveDataCounter;i++)
                {
                  if(memcmp(&receiveArray[i],"Content-Length:",sizeof("Content-Length:")-1)==0)
                  {
                    dataLen = strtol(&receiveArray[i+sizeof("Content-Length:")-1], NULL, 10);
                  }
                }

                int dataCount=0;
                while(dataCount<dataLen)
                {
                  while(client.available())
                  {
                    previousActionTimeSt = millis();
                    receiveArray[dataCount]=client.read();
                    dataCount++;
                  }
                  
                  currentTimeSt=millis();
                  if(calculateTimeSpan(currentTimeSt, previousActionTimeSt)>1000)
                  {                    
                    #ifdef SERIAL_DEBUG_ENABLED
                    Serial.println("Receive timeout 1000ms");
                    #endif
                    break;
                  }
                }
                if(dataCount<dataLen)break;

                File thresholds;
                if(!(thresholds = SPIFFS.open("/thresholds.property", "w")))
                {
                  #ifdef SERIAL_DEBUG_ENABLED
                  Serial.println("Failed to open thresholds.property to write");
                  #endif
                  break;
                }  

                for(dataCount = 2; dataCount < dataLen-(sizeof("tMin")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"tMin",sizeof("tMin")-1)==0)
                  {
                    dataCount+=(sizeof("tMin")-1+3);
                    memoryManager->tMin=atof(&receiveArray[dataCount]);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      if(receiveArray[dataCount]==',')thresholds.print('.');
                      else thresholds.print(receiveArray[dataCount]);
                      dataCount++;
                    }
                    thresholds.println();
                    break;
                  }
                }

                for(; dataCount < dataLen-(sizeof("tMax")-1);dataCount++)
                {
                  if(memcmp(&receiveArray[dataCount],"tMax",sizeof("tMax")-1)==0)
                  {
                    dataCount+=(sizeof("tMin")-1+3);
                    memoryManager->tMax=atof(&receiveArray[dataCount]);
                    while(receiveArray[dataCount]!='\"' && receiveArray[dataCount]!=0 && receiveArray[dataCount]!=',' && receiveArray[dataCount]!='}' && dataCount<dataLen)
                    {
                      if(receiveArray[dataCount]==',')thresholds.print('.');
                      else thresholds.print(receiveArray[dataCount]);
                      dataCount++;
                    }
                    thresholds.println();
                    break;
                  }
                }

                thresholds.close();

                int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",0);//at the end
                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);            
              }     
              else if(memcmp(&receiveArray[0],"GET /TMinTMax",sizeof("GET /TMinTMax")-1)==0)
              {
                File thresholds;
                if(!(thresholds = SPIFFS.open("/thresholds.property", "r")))
                {
                  #ifdef SERIAL_DEBUG_ENABLED
                  Serial.println("Failed to open thresholds.property to read");
                  #endif
                  break;
                }

                MemoryManager memoryManager;

                int numberOfBytes = sprintf(receiveArray,"{\"tMin\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&thresholds,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\",\"tMax\":\"");
                numberOfBytes+=memoryManager.readLineFromFile(&thresholds,&receiveArray[numberOfBytes]);
                numberOfBytes+=sprintf(&receiveArray[numberOfBytes],"\"}");
                receiveArray[numberOfBytes]=0;
 
                thresholds.close();                

                int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",numberOfBytes);//at the end

                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);
                client.write(receiveArray,numberOfBytes);
              }
              else if(memcmp(&receiveArray[0],"GET /Parameters",sizeof("GET /Parameters")-1)==0)
              {
                int numberOfBytes = sprintf(receiveArray
                                           ,"{\"Temperature\":\"%f\",\"Humidity\":\"%f\",\"ControlState\":\"%s\"}"
                                           ,modbusData.Temperature
                                           ,modbusData.Humidity
                                           ,modbusData.doutState==0?"Heating":modbusData.doutState==1?"Stopped":"Coolling");
                receiveArray[numberOfBytes]=0;              

                int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",numberOfBytes);//at the end

                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);
                client.write(receiveArray,numberOfBytes);
              }
              else if(memcmp(&receiveArray[0],"POST /EnableAlgo",sizeof("POST /EnableAlgo")-1)==0)
              {
                 modbusData.controlAllowed=1;
                 int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",0);//at the end
                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);
              }
              else if(memcmp(&receiveArray[0],"POST /DisableAlgo",sizeof("POST /DisableAlgo")-1)==0)
              {
                modbusData.controlAllowed=0;
                int len = sprintf(&receiveArray[RECEIVE_ARRAY_SIZE-256],PAGE_HEADER,"application/json",0);//at the end
                client.write(&receiveArray[RECEIVE_ARRAY_SIZE-256],len);
              }
              else
              {
                int len = sprintf(receiveArray,"HTTP/1.1 400\r\nServer: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
                client.write(receiveArray,len);
              }
              break;
            }
          }
          else
          {
            currentTimeSt=millis();
            if(calculateTimeSpan(currentTimeSt, previousActionTimeSt)>1000)
            {
              //Serial.println("Receive timeout 1000ms");
              break;
            }
          }        
        }

        client.flush();

        client.stop();
       }       
    }

  private:
    WiFiServer server = WiFiServer(80);
    MemoryManager* memoryManager;
    PinsStateManager* pinsStateManager;
    unsigned long currentTimeSt=0,previousActionTimeSt=0;

    /*HTTPServerMode httpServerMode = STA_MODE;*/
};

#endif
