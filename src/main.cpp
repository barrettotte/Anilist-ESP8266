#include "anilist8266.h"

/* OLED instance */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup(){
  initSerial();
  initDisplay();
  initWifi();
  delay(1500);

  if(WiFi.status() == WL_CONNECTED){
    char userQuery[512];
    WiFiClientSecure client;
    client.setInsecure(); // we're not sending anything sensitive (to allow HTTPS request)
    sprintf(userQuery, "{User(search:\"%s\"){id name statistics{anime{episodesWatched minutesWatched}}}}", _ANILIST_USER);

    // TODO: response struct
    String resp = anilistRequest(client, userQuery);
    Serial.println(resp.c_str());
    delay(500);

    StaticJsonDocument<256> respJson;
    deserializeJson(respJson, resp);
    
    AnilistUser *user = createUser(respJson["data"]);
    syncPrintfClr("user: %s\nid: %ld\nwatched:\n  %d episode(s)\n  %d day(s)\n", 
      user->username, user->userId, user->episodesWatched, user->minutesWatched/1440);
    respJson.clear(); // free up memory

    delete user;
  }
}


void loop() {
  //
}


// create Anilist user from JSON response
AnilistUser *createUser(JsonObject data){
  AnilistUser *user = (AnilistUser *) malloc(sizeof(AnilistUser));
  JsonObject stats = data["User"]["statistics"]["anime"];

  user->userId = data["User"]["id"];
  strcpy(user->username, data["User"]["name"]);
  user->episodesWatched = stats["episodesWatched"];
  user->minutesWatched = stats["minutesWatched"];
  return user;
}


// HTTP POST to Anilist GraphQL API, return response as string
String anilistRequest(WiFiClientSecure client, const char *query){
  int status = 0;
  char reqStr[512];
  String resp;
  HTTPClient http;
  StaticJsonDocument<200> reqJson;

  syncPrintfClr("Sending HTTP POST request...");
  http.begin(client, _ANILIST_HOST);
  http.addHeader("Content-Type", "application/json");
  reqJson["query"] = query;

  serializeJson(reqJson, reqStr);
  Serial.printf("HTTP REQ: %s\n", reqStr);
  status = http.POST(reqStr);
        
  if(status > 0){
    syncPrintf("\n\nHTTP POST %d\n", status);
    resp = http.getString();

    if(status != HTTP_CODE_OK){  
      Serial.printf("HTTP ERR: %s\n", resp.c_str());
    }
  } else{
    Serial.printf("HTTP FAILED: %s\n", http.errorToString(status).c_str());
  }
  http.end();
  reqJson.clear(); // free up memory
  return resp;
}


// set baud rate and init serial buffer
void initSerial(){
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();
  for(uint8_t t = 3; t > 0; t--){
    Serial.printf("WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  Serial.println("\n* * * START * * *");
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


// connect to wifi using SSID and password in include/anilist8266_config.h
void initWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(_WIFI_SSID, _WIFI_PWD);

  syncPrintf("Connecting..");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    syncPrintf(".");
  }
  syncPrintfClr("\nSSID: %s\nIP: %s\n", _WIFI_SSID, WiFi.localIP().toString().c_str());
}


// reset OLED
void clearDisplay(){
  display.clearDisplay();
  display.setCursor(0, 0);
  delay(500);
}


// clear display and print formatted string to serial + OLED
void syncPrintfClr(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  char buffer[SYNCPRINT_SIZE];
  vsnprintf(buffer, SYNCPRINT_SIZE-1, fmt, args);
  clearDisplay();
  syncPrint(buffer);
  va_end(args);
}


// print formatted string to serial + OLED
void syncPrintf(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  char buffer[SYNCPRINT_SIZE];
  vsnprintf(buffer, SYNCPRINT_SIZE-1, fmt, args);
  syncPrint(buffer);
  va_end(args);
}


// print a string to serial + OLED
void syncPrint(const char *s){
  Serial.print(s);
  display.print(s);
  display.display();
}
