#include "looking_glass.h"

weatherData weatherData;

// ---------- SETUP & LOOP HERE ---------- //
void setup() {
  Serial.begin(115200);
  connect_wifi();
  setup_ble();
}

void loop() {
  if (deviceConnected) {
    // Read messages
  }
  else {
    pServer->getAdvertising()->start();
  }
  delay(1000);
}
// --------------------------------------- //

void connect_wifi() {
  long start_wifi_millis;
  const char* ssid = "Pixel 6";
  const char* password = "";

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
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
