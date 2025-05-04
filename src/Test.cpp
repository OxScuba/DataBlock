/*
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Adafruit_GFX.h>
#include <TimeLib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>


Preferences preferences;

WiFiManager wifiManager;

TFT_eSPI tft = TFT_eSPI();  
String minerIP;

WiFiManagerParameter adressip("adressip", "AdressIP", "XXX.XXX.X.XX", 9);


Button2 button = Button2(0);
Button2 button2 = Button2(14); 

unsigned long lastDataRefresh = 0;
const unsigned long dataRefreshInterval = 10000;

unsigned long lastTimeSync = 0;
const unsigned long timeSyncInterval = 10000; 

int currentScreen = 1;

float hashrate;
float efficiency;
float temperature;
float power;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

time_t currentSystemTime = 0;


bool shouldSaveConfig = false;  

void configModeCallback(WiFiManager* myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void saveConfigToPreferences() {
  preferences.begin("wifi", false);
  preferences.putString("adressip", adressip.getValue());
  preferences.end();
}

void setup() {
    Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  tft.setSwapBytes(true);


  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("Data Block", "TicTocNextBlock")) {
    Serial.println("Failed to connect and hit timeout");
    wifiManager.startConfigPortal();
  }

  minerIP = adressip.getValue();


  button.setClickHandler([](Button2& btn) {
    currentScreen++;
    if (currentScreen > 4) {
      currentScreen = 1;
    }
    displayScreen(currentScreen);
    lastDataRefresh = 0;
  });

  button.setLongClickHandler([](Button2& btn) {
    currentScreen = 1;
    displayScreen(currentScreen);
    lastDataRefresh = 0;
  });

  button2.setClickHandler([](Button2& btn) {
    wifiManager.resetSettings();
    ESP.restart();
    
  });
}

void loop() {
  button.loop();
  button2.loop();

  if (millis() - lastDataRefresh >= dataRefreshInterval) {
    getMinerData();
    displayScreen(currentScreen);
    lastDataRefresh = millis();
  }

  if ((millis() - lastTimeSync) >= timeSyncInterval) {
    updateTime();
    lastTimeSync = millis();
  }

  if (shouldSaveConfig) {
    saveConfigCallback();
    shouldSaveConfig = false;
  }
}
/*
void displayImage(const char* filename, int displayTimeSeconds) {
  tft.pushImage(0, 0, 320, 170, b320x170_esp_data_block);
  delay(displayTimeSeconds * 1000);
}


void displayConfigScreen() {
  tft.fillScreen(TFT_WHITE);
  tft.pushImage(0, 0, 320, 170, b320x170_esp_config_wifi);
  String ssid = WiFi.SSID();
  String password = WiFi.psk();
  
  wifiManager.addParameter(&adressip);

  
  wifiManager.setClass("invert");

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("Data Block", "TicTocNextBlock")) {
    Serial.println("Failed to connect and hit timeout");
    wifiManager.startConfigPortal();
    displayConfigScreen();
  }
}

void updateTime() {
  timeClient.update();
  currentSystemTime = timeClient.getEpochTime();
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin();
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
  }
}

void displayScreen(int screenNumber) {
  switch (screenNumber) {
    case 1:
      displayScreen1();
      break;

    case 2:
      displayScreen2();
      break;

    case 3:
      displayScreen3();
      break;

    case 4:
      displayScreen4();
      break;

    default:
      break;
  }
}

void getMinerData() {
  HTTPClient http;
  String minerAPI = "http://" + minerIP + "/api/v1/status"; 
  http.begin(minerAPI);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      hashrate = doc["hashrate"]; 
      efficiency = doc["efficiency"]; 
      temperature = doc["temperature"]; 
      power = doc["power"]; 
      
    
      Serial.print("Hashrate: ");
      Serial.println(hashrate);
      Serial.print("Efficiency: ");
      Serial.println(efficiency);
      Serial.print("Temperature: ");
      Serial.println(temperature);
      Serial.print("Power: ");
      Serial.println(power);
    } else {
      Serial.println("Failed to parse JSON");
    }
  } else {
    Serial.println("Error getting data from miner");
  }
  
  http.end();
}
*/