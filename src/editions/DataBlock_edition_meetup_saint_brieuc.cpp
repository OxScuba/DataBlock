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
#include "media/320x170_pxl/320x170_esp_config_wifi.h"
#include "media/160x160_pxl/160x160_esp_meetup_saint_brieuc.h"
#include "media/320x170_pxl/320x170_esp_events_countdown_meetup_saint_brieuc.h"
#include "media/320x170_pxl/320x170_esp_mempool.h"
#include "media/320x170_pxl/320x170_esp_events_countdown.h"
#include "media/320x170_pxl/320x170_esp_lotr_shire.h"
#include "media/320x170_pxl/320x170_esp_sauron.h"

#define EEPROM_NAMESPACE "config"
Preferences preferences;

WiFiManager wifiManager;


TFT_eSPI tft;
Button2 button = Button2(0);

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

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

time_t currentSystemTime = 0;

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
  preferences.end();
}

void loadConfigFromPreferences() {
  preferences.begin(EEPROM_NAMESPACE, true);
  feesLimitBeforeMordor = preferences.getUInt("fees_limit", feesLimitBeforeMordor);
  preferences.end();
}

void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;

  feesLimitBeforeMordor = atoi(fees_limit.getValue());

  Serial.print("feesLimitBeforeMordor updated to: ");
  Serial.println(feesLimitBeforeMordor);

  saveConfigToPreferences();
}


void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  tft.setSwapBytes(true);

  displayImage("b320x170_esp_data_block", 10);

  WiFiManager wifiManager;

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
}

void loop() {
  button.loop();

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
  tft.fillScreen(TFT_WHITE);
  tft.pushImage(160, 5, 160, 160, b160x160_esp_meetup_saint_brieuc);
  tft.setTextSize(1);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(42, 40);
  tft.print("Block Height");
  tft.setCursor(60, 63);
  tft.print(blockHeight);
  tft.setTextSize(1);
  tft.setCursor(68, 86);
  tft.print("Fees");
  tft.setCursor(119, 109);
  tft.print("Low ");
  tft.setCursor(119, 132);
  tft.print(feesLow);
  tft.setCursor(61, 109);
  tft.print("Medium ");
  tft.setCursor(70, 132);
  tft.print(feesMedium);
  tft.setCursor(19, 109);
  tft.print("High ");
  tft.setCursor(20, 132);
  tft.print(feesHigh);
  tft.setCursor(14, 146);
  tft.print("sat/vB ");
  tft.setCursor(64, 146);
  tft.print("sat/vB ");
  tft.setCursor(113, 146);
  tft.print("sat/vB ");
}

void displayScreen2() {
  getMempoolData3Blocks();
  tft.fillScreen(TFT_WHITE);
  tft.pushImage(0, 0, 320, 170, b320x170_esp_mempool);
  for (int i = 0; i < 3; i++) {
    int xPosition = 143 + i * 56;
    if (i < dataSize) {
      tft.setTextSize(1);
      tft.setCursor(xPosition, 39, 2);
      tft.setTextColor(TFT_BLUE);
      tft.print(blockHeightm[i]);
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(xPosition, 39, 1);
      tft.setTextSize(1);
      tft.setCursor(xPosition + 9, 83);
      float sizeMB = size[i] / (1024.0 * 1024.0);
      tft.printf("%.1f", sizeMB);
      tft.setCursor(xPosition + 10, 68);
      tft.print(avgFeeRate[i]);
      tft.setCursor(xPosition + 10, 98);
      tft.print(txCount[i]);
      tft.setCursor(173, 68);
      tft.print("s/vB");
      tft.setCursor(230, 68);
      tft.print("s/vB");
      tft.setCursor(287, 68);
      tft.print("s/vB");
      tft.setCursor(183, 83);
      tft.print("mB");
      tft.setCursor(240, 83);
      tft.print("mB");
      tft.setCursor(297, 83);
      tft.print("mB");
      tft.setCursor(183, 98);
      tft.print("tx");
      tft.setCursor(240, 98);
      tft.print("tx");
      tft.setCursor(297, 98);
      tft.print("tx");
    } else {
      tft.setCursor(xPosition, 38);
      tft.print("N/A");
      tft.setCursor(xPosition + 9, 83);
      tft.print("N/A");
      tft.setCursor(xPosition + 10, 68);
      tft.print("N/A");
      tft.setCursor(xPosition + 10, 98);
      tft.print("N/A");
    }
  }
}

void displayScreen3() {

  updateTime();

  tft.fillScreen(TFT_WHITE);
  tft.pushImage(0, 0, 320, 170, b320x170_esp_events_countdown_meetup_saint_brieuc);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(21, 137);
  tft.print("17/01/2024");


  tmElements_t targetDateTime;
  targetDateTime.Year = 2024 - 1970;
  targetDateTime.Month = 1;
  targetDateTime.Day = 17;
  targetDateTime.Hour = 12;
  targetDateTime.Minute = 0;
  targetDateTime.Second = 0;
  time_t targetTime = makeTime(targetDateTime);

  Serial.println("Current time: " + String(ctime(&currentSystemTime)));
  Serial.println("Target time: " + String(ctime(&targetTime)));

  if (currentSystemTime < targetTime) {
    time_t timeDiff = targetTime - currentSystemTime;

    int daysRemaining = timeDiff / SECS_PER_DAY;
    int hoursRemaining = (timeDiff % SECS_PER_DAY) / SECS_PER_HOUR;
    int minutesRemaining = ((timeDiff % SECS_PER_DAY) % SECS_PER_HOUR) / SECS_PER_MIN;

    Serial.printf("Time remaining: %02d days, %02d hours, %02d minutes\n", daysRemaining, hoursRemaining, minutesRemaining);
    tft.setCursor(218, 137);
    tft.print("J-");
    tft.setCursor(252, 137);
    tft.printf("%02d", daysRemaining);
  } else {
    tft.print("Let's Go");
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