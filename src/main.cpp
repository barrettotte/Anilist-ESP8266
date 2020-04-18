#include <Arduino.h>

/* JSON support */
#include <ArduinoJson.h>

/* OLED imports */
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

/* WiFi and HTTPS requests */
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>

/* Configuration and secrets */
#include "config.h" 

// OLED dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64


void initWifi();
void initDisplay();
void clearDisplay();
void dualPrint(const String line);
void dualPrint(const String line, bool clr);


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup(){
  Serial.begin(115200);
  Serial.println("\n* * * START * * *");
  initDisplay();
  initWifi();
  delay(1500);

  if(WiFi.status() == WL_CONNECTED){
    int status = 0;
    char reqStr[256];

    StaticJsonDocument<200> reqJson;
    HTTPClient https;
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure(); // we're not sending anything sensitive, all is good

    String host = "https://graphql.anilist.co";
    //dualPrint("[HTTPS] Sending POST request...", true);

    if(https.begin(*client, host)){
      https.addHeader("Content-Type", "application/json");
      reqJson["query"] = "{User(search:\"barrettotte\"){id name statistics{anime{meanScore episodesWatched minutesWatched}}}}";
      serializeJson(reqJson, reqStr);

      Serial.printf("[HTTPS] REQ: %s\n", reqStr);
      status = https.POST(reqStr);
        
      if(status > 0){
        const String& resp = https.getString();
        //dualPrint("\n\n[HTTPS] POST " + String(status) + "\n");
        Serial.printf("[HTTPS] POST %d\n", status);

        if(status == HTTP_CODE_OK){  
          Serial.printf("[HTTPS] RESP: %s\n", resp.c_str());

        } else if(status >= 400){
          Serial.printf("[HTTPS] ERR: %s\n", resp.c_str());
        }
      } else{
        Serial.printf("[HTTPS] FAILED: %s\n", https.errorToString(status).c_str());
      }
      https.end();
    } else{
      Serial.println("[HTTPS] Unable to connect");
    }
  }
}

void loop() {
  //
}


// connect to wifi using SSID and password in include/config.h
void initWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(_WIFI_SSID, _WIFI_PWD);

  dualPrint("Connecting..");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    //dualPrint(".");
    Serial.print(".");
  }
  //dualPrint("SSID: " + _WIFI_SSID + "\n\rIP: " + WiFi.localIP().toString(), true);
  Serial.println("SSID: " + _WIFI_SSID + "\nIP: " + WiFi.localIP().toString());
}


// setup OLED display and text properties
void initDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1500);
  clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}


// reset OLED
void clearDisplay(){
  display.clearDisplay();
  display.setCursor(0, 0);
  delay(500);
}


// print to serial and OLED in one go
void dualPrint(const String line){
  dualPrint(line, false);
}

void dualPrint(const String line, bool clr){
  if(clr){
    clearDisplay();
  }
  Serial.print(line);
  display.print(line);
  display.display();
}
