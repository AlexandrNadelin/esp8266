#ifndef __WIFI_MANAGER_H
#define __WIFI_MANAGER_H
#include <Arduino.h>
#include "Config.h"
#include "HTTPServer.h"
#include "ModbusTCPServer.h"

class WIFIManager
{
  public:
    WIFIManager(){}
    ~WIFIManager(){}

    void begin(MemoryManager* memoryManager,PinsStateManager* pinsStateManager)
    { 
      this->memoryManager=memoryManager;
      this->pinsStateManager = pinsStateManager;
      
      if(pinsStateManager->DINModeState)
      {
        WiFi.mode(WIFI_STA);
        WiFi.begin(memoryManager->networkProperty.stSSID,memoryManager->networkProperty.stPassword);
        IPAddress stIP;
        IPAddress stGateWay;
        IPAddress stSubnet;
        stIP.fromString(memoryManager->networkProperty.stIP);
        stGateWay.fromString(memoryManager->networkProperty.stGateWay);
        stSubnet.fromString(memoryManager->networkProperty.stSubnet);
        WiFi.config(stIP,stGateWay,stSubnet);

        httpServer.begin(memoryManager,pinsStateManager);

        modbusTCPServer.infoRegisters=&infoRegisters;
        modbusTCPServer.inputRegisters=&inputRegisters;
        modbusTCPServer.contactRegisters=NULL;//&contactRegisters;
        modbusTCPServer.coilRegisters=&coilRegisters;
        modbusTCPServer.begin(memoryManager,pinsStateManager);
      }
      else 
      {
        WiFi.mode(WIFI_AP);
        IPAddress apIP;
        IPAddress apGateWay;
        IPAddress apSubnet;      
        apIP.fromString(memoryManager->networkProperty.apIP);
        apGateWay.fromString(memoryManager->networkProperty.apGateWay);
        apSubnet.fromString(memoryManager->networkProperty.apSubnet);
      
        #ifdef SERIAL_DEBUG_ENABLED
          Serial.print("Setting soft-AP configuration ... ");
          Serial.println(WiFi.softAPConfig(apIP, apGateWay, apSubnet) ? "Ready" : "Failed!");
        #else      
          bool result = WiFi.softAPConfig(apIP, apGateWay, apSubnet);
        #endif

        #ifdef SERIAL_DEBUG_ENABLED
          Serial.print("Setting soft-AP ... ");
          Serial.println(WiFi.softAP(memoryManager->networkProperty.apSSID,memoryManager->networkProperty.apPassword) ? "Ready" : "Failed!");
        #else
          result = WiFi.softAP(memoryManager->networkProperty.apSSID,memoryManager->networkProperty.apPassword);
        #endif
      
        #ifdef SERIAL_DEBUG_ENABLED
          Serial.print("Soft-AP IP address = ");
          Serial.println(WiFi.softAPIP());
        #endif
        httpServer.begin(memoryManager,pinsStateManager);
        /*modbusTCPServer.begin(memoryManager,pinsStateManager);*/
      }
    }

    void loop()
    {
      if(pinsStateManager->DINModeState)
      {
        if(WiFi.status() != WL_CONNECTED)
        { 
          #ifdef SERIAL_DEBUG_ENABLED   
            Serial.print("Connecting to ");
            Serial.println(memoryManager->networkProperty.stSSID);
          #endif
          WiFi.begin(memoryManager->networkProperty.stSSID, memoryManager->networkProperty.stPassword);
          if (WiFi.waitForConnectResult() != WL_CONNECTED)return;
          #ifdef SERIAL_DEBUG_ENABLED
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
          #endif  
        }
        else 
        {
          httpServer.loop();
          modbusTCPServer.loop();
        }
      }
      else httpServer.loop();
    }
    
    private:
    MemoryManager* memoryManager;
    PinsStateManager* pinsStateManager;
    HTTPServer httpServer;     
    ModbusTCPServer modbusTCPServer;
};







#endif
