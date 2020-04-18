#ifndef ANILIST8266_H
#define ANILIST8266_H

#include <Arduino.h>

/* JSON support */
#include <ArduinoJson.h>

/* OLED support */
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

/* WiFi and HTTPS requests */
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>  // create TLS connection

/* std includes */
#include <stdarg.h> // variable length args

/* Configuration and secrets */
#include "anilist8266_config.h"

/* OLED dimensions */
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

#define SYNCPRINT_SIZE 256
const char *_ANILIST_HOST = "https://graphql.anilist.co";


typedef struct anilistUser{
    long userId;
    char username[128];
    int episodesWatched;
    int minutesWatched;    
} AnilistUser;

/* main */
void initSerial();
void initWifi();
void initDisplay();

String anilistRequest(WiFiClientSecure client, const char *query);
AnilistUser *createUser(JsonObject data);

/* display utils */
void clearDisplay();
void syncPrint(const char *s);
void syncPrintf(const char *fmt, ...);
void syncPrintfClr(const char *fmt, ...);

#endif