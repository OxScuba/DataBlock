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

#include "media/320x170_pxl/320x170_esp_data_block.h"
#include "media/320x170_pxl/320x170_esp_sauron.h"
#include "media/320x170_pxl/320x170_esp_mordor_mempol.h"
#include "media/320x170_pxl/320x170_esp_config_wifi.h"
#include "media/320x170_pxl/320x170_esp_lotr_shire.h"
#include "media/320x170_pxl/320x170_esp_lotr_gondor.h"
#include "media/320x170_pxl/320x170_esp_lotr.h"

#define EEPROM_NAMESPACE "config"

Preferences preferences;

WiFiManager wifiManager;

TFT_eSPI tft;
Button2 button = Button2(0);
Button2 button2 = Button2(14); 

const String mempoolAPIFees = "https://mempool.space/api/v1/fees/recommended";
const String mempoolAPIBlockHeight = "https://mempool.space/api/blocks/tip/height";
const char* mempoolAPI3Blocks = "https://mempool.space/api/v1/blocks/";

unsigned long lastDataRefresh = 0;
const unsigned long dataRefreshInterval = 10000;

unsigned long lastTimeSync = 0;
const unsigned long timeSyncInterval = 10000; 

unsigned long blockHeight;
int feesLow;
int feesMedium;
int feesHigh;

unsigned long blockHeightm[3];
float size[3];
int avgFeeRate[3];
int txCount[3];
int dataSize;

int currentScreen = 1;

int feesLimitBeforeMordor = 50;

WiFiManagerParameter fees_limit("feesLimit", "Fees Limit Before Mordor", String(feesLimitBeforeMordor).c_str(), 4);
WiFiManagerParameter day_param("day", "Day", "3", 2);
WiFiManagerParameter month_param("month", "Month", "1", 2);
WiFiManagerParameter year_param("year", "Year", "2008", 4);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

time_t currentSystemTime = 0;
tmElements_t targetDateTime;

bool shouldSaveConfig = false;  

void saveConfigToPreferences();
void loadConfigFromPreferences();
void saveConfigCallback();
void setup();
void loop();
void displayImage(const char* filename, int displayTimeSeconds);
void displayConfigScreen();
void updateTime();
void connectToWiFi();
void displayScreen(int screenNumber);
void displayScreen1();
void displayScreen2();
void displayScreen3();
void displayScreen4();
void getMempoolDataFees();
void getMempoolData3Blocks();
void getMempoolDataBlockHeight();


void configModeCallback(WiFiManager* myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void saveConfigToPreferences() {
  preferences.begin(EEPROM_NAMESPACE, false);
  preferences.putUInt("fees_limit", feesLimitBeforeMordor);

  preferences.putUInt("day", targetDateTime.Day);
  preferences.putUInt("month", targetDateTime.Month);
  preferences.putUInt("year", targetDateTime.Year + 1970);

  preferences.end();
}

void loadConfigFromPreferences() {
  preferences.begin(EEPROM_NAMESPACE, true);
  feesLimitBeforeMordor = preferences.getUInt("fees_limit", feesLimitBeforeMordor);

  targetDateTime.Day = preferences.getUInt("day", 3);
  targetDateTime.Month = preferences.getUInt("month", 1);
  targetDateTime.Year = preferences.getUInt("year", 2008) - 1970;

  preferences.end();
}

void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;

  feesLimitBeforeMordor = atoi(fees_limit.getValue());

  
  targetDateTime.Day = atoi(day_param.getValue());
  targetDateTime.Month = atoi(month_param.getValue());
  targetDateTime.Year = atoi(year_param.getValue()) - 1970;

  Serial.print("feesLimitBeforeMordor updated to: ");
  Serial.println(feesLimitBeforeMordor);

  Serial.print("Day updated to: ");
  Serial.println(targetDateTime.Day);

  Serial.print("Month updated to: ");
  Serial.println(targetDateTime.Month);

  Serial.print("Year updated to: ");
  Serial.println(targetDateTime.Year);

  saveConfigToPreferences();
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  tft.setSwapBytes(true);

  displayImage("b320x170_esp_data_block", 10);

  displayConfigScreen();

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("Data Block", "TicTocNextBlock")) {
    Serial.println("Failed to connect and hit timeout");
    wifiManager.startConfigPortal();
    displayConfigScreen();
  }

  connectToWiFi();

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

void displayImage(const char* filename, int displayTimeSeconds) {
  tft.pushImage(0, 0, 320, 170, b320x170_esp_data_block);
  delay(displayTimeSeconds * 1000);
}

void displayConfigScreen() {
  tft.fillScreen(TFT_WHITE);
  tft.pushImage(0, 0, 320, 170, b320x170_esp_config_wifi);
  String ssid = WiFi.SSID();
  String password = WiFi.psk();
  
  wifiManager.addParameter(&fees_limit);
  
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

void displayScreen1() {
      getMempoolDataFees();
      getMempoolDataBlockHeight();
      tft.fillScreen(TFT_BLACK);
      tft.pushImage(0, 0, 320, 170, b320x170_esp_lotr);
      tft.setTextSize(1);
      tft.setTextColor(TFT_GOLD);
      tft.setCursor(128, 72);
      tft.print("Block Height");
      tft.setCursor(143, 92);
      tft.print(blockHeight);
}

void displayScreen2() {
      getMempoolDataFees();
      getMempoolDataBlockHeight();
      tft.fillScreen(TFT_WHITE);
      tft.pushImage(0, 0, 320, 170, b320x170_esp_lotr_gondor);
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(42, 40);
      tft.print("Block Height");
      tft.setCursor(60, 63);
      tft.print(blockHeight);
      tft.setTextSize(1);
      tft.setCursor(215, 115);
      tft.print("Fees");
      tft.setCursor(266, 129);
      tft.print("Low ");
      tft.setCursor(266, 143);
      tft.print(feesLow);
      tft.setCursor(206, 129);
      tft.print("Medium ");
      tft.setCursor(216, 143);
      tft.print(feesMedium);
      tft.setCursor(165, 129);
      tft.print("High ");
      tft.setCursor(166, 143);
      tft.print(feesHigh);
      tft.setCursor(160, 157);
      tft.print("sat/vB ");
      tft.setCursor(210, 157);
      tft.print("sat/vB ");
      tft.setCursor(260, 157);
      tft.print("sat/vB ");
}

void displayScreen3() {
  getMempoolData3Blocks();
  tft.fillScreen(TFT_WHITE);
  tft.pushImage(0, 0, 320, 170, b320x170_esp_mordor_mempol);  

  for (int i = 0; i < 3; i++) {
    int xPosition = 143 + i * 56;
    if (i < dataSize) {
      tft.setTextSize(1);
      tft.setCursor(xPosition, 84, 2);  
      tft.setTextColor(TFT_BLUE);
      tft.print(blockHeightm[i]);
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(xPosition, 84, 1);  
      tft.setTextSize(1);
      tft.setCursor(xPosition + 9, 128);  
      float sizeMB = size[i] / (1024.0 * 1024.0);
      tft.printf("%.1f", sizeMB);
      tft.setCursor(xPosition + 10, 113); 
      tft.print(avgFeeRate[i]);
      tft.setCursor(xPosition + 10, 143);  
      tft.print(txCount[i]);
      tft.setCursor(173, 113);  
      tft.print("s/vB");
      tft.setCursor(230, 113);  
      tft.print("s/vB");
      tft.setCursor(287, 113);  
      tft.print("s/vB");
      tft.setCursor(183, 128);  
      tft.print("mB");
      tft.setCursor(240, 128);  
      tft.print("mB");
      tft.setCursor(297, 128);  
      tft.print("mB");
      tft.setCursor(183, 143);  
      tft.print("tx");
      tft.setCursor(240, 143);  
      tft.print("tx");
      tft.setCursor(297, 143);  
      tft.print("tx");
    } else {
      tft.setCursor(xPosition, 83);  
      tft.print("N/A");
      tft.setCursor(xPosition + 9, 128);  
      tft.print("N/A");
      tft.setCursor(xPosition + 10, 113);  
      tft.print("N/A");
      tft.setCursor(xPosition + 10, 143);  
      tft.print("N/A");
    }
  }
}

void displayScreen4() {
  getMempoolDataFees();  

  tft.fillScreen(TFT_WHITE); 

  if (feesMedium > feesLimitBeforeMordor) {
    tft.pushImage(0, 0, 320, 170, b320x170_esp_sauron);
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.setCursor(271, 20);
    tft.print(feesMedium);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(246, 40);
    tft.print("sat/vB");
  } else {
    tft.pushImage(0, 0, 320, 170, b320x170_esp_lotr_shire);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.setCursor(271, 20);
    tft.print(feesMedium);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(246, 40);
    tft.print("sat/vB");
  }
}

void getMempoolDataFees() {
  HTTPClient http;
  http.begin(mempoolAPIFees);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    feesLow = doc["hourFee"];
    feesMedium = doc["halfHourFee"];
    feesHigh = doc["fastestFee"];
  } else {
    Serial.println("Error getting data from mempool.space");
  }
  http.end();
}

void getMempoolData3Blocks() {
  HTTPClient http;
  http.begin(mempoolAPI3Blocks);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(8192);
    deserializeJson(doc, payload);
    serializeJsonPretty(doc, Serial);
    Serial.println();
    dataSize = (3 < doc.size()) ? 3 : doc.size();
    for (int i = 0; i < dataSize; i++) {
      blockHeightm[i] = doc[i]["height"].as<unsigned long>();
      size[i] = doc[i]["size"].as<int>();
      avgFeeRate[i] = doc[i]["extras"]["medianFee"].as<int>();
      txCount[i] = doc[i]["tx_count"].as<int>();
    }
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();
}

void getMempoolDataBlockHeight() {
  HTTPClient http;
  http.begin(mempoolAPIBlockHeight);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    String blockHeightStr = payload;
    blockHeight = blockHeightStr.toInt();
  } else {
    Serial.println("Error getting data from mempool.space");
  }
  http.end();
}
*/