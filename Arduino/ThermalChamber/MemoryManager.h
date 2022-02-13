#ifndef __MEMORY_MANAGER_H
#define __MEMORY_MANAGER_H
#include <Arduino.h>
#include "Config.h"

struct NetworkProperty
{
  char apSSID[28];
  char apPassword[28];
  char apIP[16];
  char apSubnet[16];
  char apGateWay[16];
  char modbusTCPPort[6];
  
  char stSSID[28];
  char stPassword[28];
  char stIP[16];
  char stSubnet[16];
  char stGateWay[16];
};

const char apSSID[] PROGMEM =       "R_04_LOW_COST";//"\"apSSID\": \"R_04_LOW_COST\",";
const char apPassword[] PROGMEM =   "AngryBird";//"\"apPassword\": \"AngryBird\",";
const char apIP[] PROGMEM =         "192.168.5.1";//"\"apIP\": \"192.168.5.1\",";
const char apSubnet[] PROGMEM =     "255.255.255.0";//"\"apSubnet\": \"255.255.255.0\",";
const char apGateWay[] PROGMEM =    "192.168.5.1";//"\"apGateWay\": \"192.168.5.1\",";
const char modbusTCPPort[] PROGMEM =   "502";

const char stSSID[] PROGMEM =       "TP-LINK_453D2A";//"\"stSSID\": \"TP-LINK_453D2A\",,);/
const char stPassword[] PROGMEM =   "36216703";//"\"stPassword\": \"36216703\",";
const char stIP[] PROGMEM =         "192.168.4.225";//"\"apIP\": \"192.168.5.1\",";
const char stSubnet[] PROGMEM =     "255.255.255.0";//"\"apSubnet\": \"255.255.255.0\",";
const char stGateWay[] PROGMEM =    "192.168.4.1";//"\"apGateWay\": \"192.168.5.1\",";

static const int arrayPropertiesLenght = 11;

const char* const arrayProperties[] PROGMEM = {
  apSSID, apPassword, apIP,apSubnet,apGateWay,modbusTCPPort,stSSID,stPassword, stIP,stSubnet,stGateWay,
};

class MemoryManager {    
  public:
    MemoryManager(){}//constructor
    ~MemoryManager(){}//destructor
  
    int readLineFromFile(File* file,char* destString)
    {
      byte byteTmp=0;
      int currentByte = 0; 
      while (file->available())
      {//int l = file.readBytesUntil('\n', buffer, sizeof(buffer));
        byteTmp=file->read(); 
        if((char)byteTmp=='\n')break;
        else if((char)byteTmp!='\r') destString[currentByte++] = byteTmp;//'\r' skip
      } 
      destString[currentByte] = 0;
      return currentByte; 
    }

    bool writeDefaultNetworkParametersToFile()
    {
      File networkParameters;
      if(!(networkParameters = SPIFFS.open("/networkParameters.property", "w")))
      {
        #ifdef SERIAL_DEBUG_ENABLED
        Serial.println("Failed to open networkParameters.property to write");
        #endif
        return false;
      }
      for(int j = 0; j < arrayPropertiesLenght;j++)
      {
        for(int i = 0; i < strlen_P(arrayProperties[j]); i++)
        {
          networkParameters.print((char)pgm_read_byte(&arrayProperties[j][i]));
        }
        networkParameters.println();
      }
 
      networkParameters.close(); 
      return true;
    }

    bool writeDefaultThresholdsToFile()
    {
      File thresholds;
      if(!(thresholds = SPIFFS.open("/thresholds.property", "w")))
      {
        #ifdef SERIAL_DEBUG_ENABLED
          Serial.println("Failed to open thresholds.property to write");
        #endif
        return false;
      }
      thresholds.println("-126.0"); 
      thresholds.println("127.0"); 
      thresholds.close(); 
      return true;
    }
    
	  void begin()
    {     
      bool resultMountinFileSystem = SPIFFS.begin();
 
      if (resultMountinFileSystem)
      {
        #ifdef SERIAL_DEBUG_ENABLED
        Serial.println("File system mounted with success");
        #endif
      }
      else 
      {
        #ifdef SERIAL_DEBUG_ENABLED
        Serial.println("Error mounting the file system");
        #endif
        return;
      } 

      File networkParameters;  
      while(!(networkParameters = SPIFFS.open("/networkParameters.property", "r")))
      {
        #ifdef SERIAL_DEBUG_ENABLED
        Serial.println("Failed to open networkParameters.property to read");
        #endif

        if(!writeDefaultNetworkParametersToFile())return;
      }

      //Если не получится получить все данные - надо записать дефолтные значения
  
      if(readLineFromFile(&networkParameters,networkProperty.apSSID)==0
       ||readLineFromFile(&networkParameters,networkProperty.apPassword)==0
       ||readLineFromFile(&networkParameters,networkProperty.apIP)==0
       ||readLineFromFile(&networkParameters,networkProperty.apSubnet)==0
       ||readLineFromFile(&networkParameters,networkProperty.apGateWay)==0
       ||readLineFromFile(&networkParameters,networkProperty.modbusTCPPort)==0
  
       ||readLineFromFile(&networkParameters,networkProperty.stSSID)==0
       ||readLineFromFile(&networkParameters,networkProperty.stPassword)==0
       ||readLineFromFile(&networkParameters,networkProperty.stIP)==0
       ||readLineFromFile(&networkParameters,networkProperty.stSubnet)==0
       ||readLineFromFile(&networkParameters,networkProperty.stGateWay)==0)
       {
         #ifdef SERIAL_DEBUG_ENABLED
         Serial.println("networkParameters.property reading error, write default settings");
         #endif
         networkParameters.close();
         writeDefaultNetworkParametersToFile();
       }
       else networkParameters.close();
  
       File thresholds;
       while(!(thresholds = SPIFFS.open("/thresholds.property", "r")))
       {
         #ifdef SERIAL_DEBUG_ENABLED
           Serial.println("Failed to open thresholds.property to read");
         #endif

         if(!writeDefaultThresholdsToFile())return;
       }
       
       char thresholdsTMinStr[16];
       char thresholdsTMaxStr[16];
       if(readLineFromFile(&thresholds,thresholdsTMinStr)==0||readLineFromFile(&thresholds,thresholdsTMaxStr)==0)
       {
         #ifdef SERIAL_DEBUG_ENABLED
           Serial.println("thresholds.property reading error, write default settings");
         #endif         
         
         thresholds.close();
         writeDefaultThresholdsToFile();
         tMin=-126.0;
         tMax=127.0;
       }
       else 
       {
        thresholds.close();
        tMin = atof(thresholdsTMinStr);
        tMax = atof(thresholdsTMaxStr);
       }
       
  
        /*Serial.println(networkProperty.apSSID);
      Serial.println(networkProperty.apPassword);
      Serial.println(networkProperty.apIP);
      Serial.println(networkProperty.apSubnet);
      Serial.println(networkProperty.apGateWay);
      Serial.println(networkProperty.modbusTCPPort);
  
      Serial.println(networkProperty.stSSID);
      Serial.println(networkProperty.stPassword);
      Serial.println(networkProperty.stIP);
      Serial.println(networkProperty.stSubnet);
      Serial.println(networkProperty.stGateWay);*/

    }
    
    NetworkProperty networkProperty;
    float tMin=-126.0;
    float tMax=127.0;
  private:
  
};


#endif
