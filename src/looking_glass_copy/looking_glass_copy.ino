#include "looking_glass_copy.h"

weatherData weatherData;

// ---------- SETUP & LOOP HERE ---------- //
void setup() {
//  Serial.begin(115200);

//  setup_ble();
  nexInit();
  analogReadResolution(12);
  next = millis();
  connect_wifi();
}

void loop() {
//  if (deviceConnected) {
//    // Read messages
//    nexLoop(nex_listen_list); // what does this actually do if the nex_listen_list is empty
//    if (millis() >= next) {
//      next = millis()+500;
//      do_every_so_often();
//    }
//    
//    int pot_analog_val = analogRead(33);
//    int pot_analog_volts = analogReadMilliVolts(33);
//    ADC_an_val.setValue(pot_analog_val);
//    ADC_v_val.setValue(pot_analog_volts);
//    Terminal.setText("The FitnessGram Pacer Test is a multistage \r\naerobic capacity test that aggressively gets \r\nmore difficult as it continues. \r\nThe 20 meter pacer test will begin \r\nin 30 seconds. Line up at the start. \r\nThe running speed starts slowly, \r\nbut gets faster each minute \r\nafter you hear this signal. \r\n[beep]. A single lap should \r\nbe completed each time you \r\nhear this sound. [ding]. \r\nRemember to run in a \r\nstraight line, and run as \r\nlong as possible. The \r\nsecond time you fail to \r\ncomplete a lap before the sound, \r\nyour test is over. The \r\ntest will begin on the word \r\nstart. On your mark, get \r\nready, start.");
//    delay(100);
//  }
//  else {
//    pServer->getAdvertising()->start();
//  }

  // Read messages
  nexLoop(nex_listen_list); // what does this actually do if the nex_listen_list is empty
  if (millis() >= next) {
    next = millis()+500;
    do_every_so_often();
  }
  
  int pot_analog_val = analogRead(33);
  int pot_analog_volts = analogReadMilliVolts(33);
  ADC_an_val.setValue(pot_analog_val);
  ADC_v_val.setValue(pot_analog_volts);
  Terminal.setText("The FitnessGram Pacer Test is a multistage \r\naerobic capacity test that aggressively gets \r\nmore difficult as it continues. \r\nThe 20 meter pacer test will begin \r\nin 30 seconds. Line up at the start. \r\nThe running speed starts slowly, \r\nbut gets faster each minute \r\nafter you hear this signal. \r\n[beep]. A single lap should \r\nbe completed each time you \r\nhear this sound. [ding]. \r\nRemember to run in a \r\nstraight line, and run as \r\nlong as possible. The \r\nsecond time you fail to \r\ncomplete a lap before the sound, \r\nyour test is over. The \r\ntest will begin on the word \r\nstart. On your mark, get \r\nready, start.");
  delay(1000);
}
// --------------------------------------- //

void connect_wifi() {
  long start_wifi_millis;
  const char* ssid = "Pixel 6";
  const char* password = "";
  String test = "";

  p2.show();

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  DebugPage.setText("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    test += ".";
    Serial.print(".");
    const char* temporary = test.c_str();
    DebugPage.setText(temporary);
  }
  p0.show();
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  LightSenVal.setText("Connected to Wifi with IP Address: " + WiFi.localIP());
  Serial.println(WiFi.localIP());
  delay(1000);
}

void setup_ble() {
  BLEDevice::init("Looking Glass");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void scan_wifi() {
  String ssids_array[20];
  String network_string;
  String connected_string;
  
  WiFi.mode(WIFI_STA);
  int n =  WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.println();
    Serial.print(n);
    Serial.println(" networks found");
    delay(1000);
    for (int i = 0; i < n; ++i) {
      ssids_array[i + 1] = WiFi.SSID(i);
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(ssids_array[i + 1]);
    }
    wifi_stage = SCAN_COMPLETE;
  }
}

void get_request(String request) {
  String path = "";
  String server;
  String resource;
  String key;
  
  if (request == "News") {
    server = "https://newsapi.org/";
    resource = "v2/top-headlines?country=us&pageSize=3&apiKey=";
    key = "45dfabcb83944dd2be7bdae84dd2b58c";
  }
  else if (request == "Weather") {
    server = "https://api.openweathermap.org/";
    resource = "data/2.5/weather?q=Lafayette,US&units=imperial&APPID=";
    key = "83ceb2a0db4ea7956827180d56992497";
  }

  path = server + resource + key;

  HTTPClient http;
  http.begin(path);
  int httpResponseCode = http.GET();
  String payload;

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  if (request == "Weather") {
    readWeatherResponseContent(&weatherData, payload);
    printWeatherData(&weatherData);
  }
}

bool readWeatherResponseContent(struct weatherData* clientData, String payload) {
  const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) 
      + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) 
      + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + 985;

  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(payload);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }

  clientData->tempCur = root["main"]["temp"];
  clientData->tempHgh = root["main"]["temp_max"];
  clientData->tempLow = root["main"]["temp_min"];
  clientData->condition = root["weather"]["main"];
  return true;
}

void printWeatherData(const struct weatherData* clientData) {
  Serial.print("Current Temp = ");
  Serial.println(clientData->tempCur);
  Serial.print("High Temp = ");
  Serial.println(clientData->tempHgh);
  Serial.print("Low Temp = ");
  Serial.println(clientData->tempLow);
  Serial.print("Condition: ");
  Serial.println(clientData->condition);
}

void do_every_so_often() {
  if (myInt == 10 or myInt == 30 or myInt == 50 or myInt == 70 or myInt == 90) {
    p1.show();
  } else if (myInt == 0 or myInt == 20 or myInt == 40 or myInt == 60 or myInt == 80) {
    p0.show();
  } else {}
  
  myInt = (myInt + 1) % 101;
  MyIntCount.setValue(myInt);
}
