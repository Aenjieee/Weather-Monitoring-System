#include <WiFi.h>
#include "time.h"
#include "sntp.h"
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Back_Space"; // Wi-Fi SSID
const char* password = "Grateful24"; // Wi-Fi password

// Define your NTP servers and offsets
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// LCD Object
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Weather API credentials and location
String URL = "http://api.openweathermap.org/data/2.5/weather?";
String ApiKey = "2f9bc88763f373d401cc46c78615434d";
String lat = "13.139300344338212";
String lon = "123.74519344524195";

void printDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }

  // Display Date Only
  lcd.clear();
  lcd.print(&timeinfo, "%B %d %Y");  // November 22 2022
}

void setup() {
  Serial.begin(115200);

  // Setup LCD with backlight and initialize
  lcd.init();
  lcd.backlight();

  // Set notification callback function
  sntp_set_time_sync_notification_cb(timeavailable);

  // NTP server address could be acquired via DHCP
  sntp_servermode_dhcp(1);

  // Set configured NTP servers and constant TimeZone/daylightOffset
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  // Connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  lcd.clear();
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
  lcd.clear();
  lcd.print("CONNECTED");
  delay(2000);
}

void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printDate();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Set HTTP Request Final URL with Location and API key information
    http.begin(URL + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + ApiKey);
    String ip = URL + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + ApiKey;
    Serial.print(ip);
    int httpCode = http.GET();

    if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
      String JSON_Data = http.getString();
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, JSON_Data);
      JsonObject obj = doc.as<JsonObject>();

      const char* description = obj["weather"][0]["description"].as<const char*>();
      float temp = obj["main"]["temp"].as<float>();
      float humidity = obj["main"]["humidity"].as<float>();

      // Serial.println('Desc: ' + description);
      // Serial.println('Temp: ' + temp);
      // Serial.println('Humidity: ' + humidity);
      // Display Weather Info on LCD
      lcd.clear();
      lcd.print("Desc: ");
      lcd.setCursor(0, 1);
      lcd.print(description);
      delay(2000); // Display description for 2 seconds

      lcd.clear();
      lcd.print("Temp: ");
      lcd.setCursor(0, 1);
      lcd.print(temp);
      lcd.print(" C");
      delay(2000); // Display temperature for 2 seconds

      lcd.clear();
      lcd.print("Humidity: ");
      lcd.setCursor(0, 1);
      lcd.print(humidity);
      lcd.print(" %");
      delay(2000); // Display humidity for 2 seconds

      // Display Date
      printDate();
    } else {
      Serial.println("Error!");
      Serial.println("Can't Get DATA!");
    }
//updated
    http.end();
  }

  delay(1000); // Wait for a second
}

//Updated Files 
