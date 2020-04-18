#include <Arduino.h>

/* OLED imports */
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

/* WiFi and HTTP requests */
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>

#include "config.h" // WiFi credentials, Anilist user target

// OLED dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64


void initSerial();
void initWifi();
void initDisplay();

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup(){
  initSerial();
  initWifi();
  initDisplay();
  delay(100);
}

void loop() {

  display.println("Hello from VS Code");
  if(WiFi.status() == WL_CONNECTED){
    int status = 0;
    WiFiClient client;
    HTTPClient http;

    http.begin(client, "https://graphql.anilist.co");
    http.addHeader("Content-Type", "application/json");

    String userQuery = "{User(search:\"barrettotte\"){id name statistics{anime{meanScore episodesWatched minutesWatched}}}}";
    status = http.POST("{\"query\": \"" + userQuery + "\"}");
    
    if(status > 0){
      const String& resp = http.getString();
      Serial.printf("[HTTP] POST (%d)\n", status);

      if(status == HTTP_CODE_OK){  
        Serial.printf("[HTTP] SUCCESS:\n%s\n", resp.c_str());
      } else if(status >= 400){
        Serial.printf("[HTTP] FAILED:\n%s\n", resp.c_str());
      }
    } else{
      Serial.printf("[HTTP] POST FAILED; ERR: %s\n", http.errorToString(status).c_str());
    }
    http.end();
  }
  delay(50000);
  display.clearDisplay();
}


// setup serial monitor
void initSerial(){
  Serial.begin(115200);
  
  Serial.println();
  Serial.println();
  Serial.println();

  for(uint8_t t = 4; t > 0; t--){
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
}


// connect to wifi using SSID and password in include/config.h
void initWifi(){
  WiFi.begin(_WIFI_SSID, _WIFI_PWD);
  Serial.print("Connecting..");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected to %s.\n", _WIFI_SSID.c_str());
}


// setup OLED display and text properties
void initDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("[DSPLY] SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.display();
}
