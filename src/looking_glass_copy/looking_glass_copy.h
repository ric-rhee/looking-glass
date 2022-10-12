#include <ArduinoJson.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include "BluetoothSerial.h"
#include "Nextion.h"

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

NexPage p0 = NexPage(0,0,"page0");
NexPage p1 = NexPage(1,0,"page1");
NexPage p2 = NexPage(2,0,"page2");
NexText DebugPage = NexText(2,2,"tDebug");
NexText CurrTempVal = NexText(0,7,"tCurrTempVal");
NexText HighTempVal = NexText(0,8,"tHighTempVal");
NexText LowTempVal = NexText(0,9,"tLowTempVal");
NexText MotSenVal = NexText(0,10,"tMotSenVal");
NexText LightSenVal = NexText(0,11,"tLightSenVal");
NexNumber MyIntCount = NexNumber(0,12,"nMyIntCount");
NexNumber ADC_an_val = NexNumber(0,15,"nADCanval");
NexNumber ADC_v_val = NexNumber(0,17,"nADCvval");

NexText Terminal = NexText(1,3,"tTerminal");
NexTouch *nex_listen_list[] = {NULL};

uint32_t next, myInt = 0;

void scan_wifi();
void connect_wifi();
void get_request(String);
void do_every_so_often();

BluetoothSerial SerialBT;

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

enum wifi_setup_stages {NONE, 
                        SCAN_START, 
                        SCAN_COMPLETE, 
                        SSID_ENTERED, 
                        WAIT_PASS, 
                        PASS_ENTERED, 
                        WAIT_CONNECT, 
                        LOGIN_FAILED};
enum wifi_setup_stages wifi_stage = NONE;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");

      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
      }

      Serial.println();

      // Do stuff based on the command received from the app      
      if (rxValue.find("Weather") != -1) {
        Serial.println("Fetching weather data...");
        get_request("Weather");
      }
      else if (rxValue.find("Time") != -1) {
        Serial.println("The current time is:");
      }
      else if (rxValue.find("Scan") != -1) {
        scan_wifi();
      }
      else if (rxValue.find("News") != -1) {
        get_request("News");
      }
      else if (rxValue.find("Test") != -1) {
        pCharacteristic->setValue("Hello World!");
        pCharacteristic->notify();
      }
    }
  }
};

struct weatherData {
  const char* tempCur;
  const char* tempHgh;
  const char* tempLow;
  const char* condition;
};
