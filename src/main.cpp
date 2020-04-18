#include "anilist8266.h"

/* globals */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); /* OLED instance */
WiFiClientSecure client;
char queryBuffer[512];

void setup(){
  initSerial();
  initDisplay();
  initWifi();
  delay(1500);

  client.setInsecure(); // we're not sending anything sensitive (to allow HTTPS request)
  sprintf(queryBuffer, 
    "{User(search:\"%s\"){id name statistics{anime{episodesWatched minutesWatched}}}}", _ANILIST_USER);
}


void loop() {
  if(WiFi.status() == WL_CONNECTED){
    GraphqlQuery *graphql = anilistRequest(client, queryBuffer);
    Serial.println(graphql->resp);
    delay(1000);

    StaticJsonDocument<256> respJson;
    deserializeJson(respJson, graphql->resp);
    delete graphql;
    
    AnilistUser *user = createUser(respJson["data"]);
    syncPrintfClr("user: %s\nid:   %ld\n\nwatched:\n\n  %d episode(s)\n\n  %d day(s)\n", 
      user->username, user->userId, user->episodesWatched, user->minutesWatched/1440);
    
    respJson.clear();
    delete user;
  }
  delay(3600000); // once an hour
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


// HTTP POST to Anilist GraphQL API
GraphqlQuery *anilistRequest(WiFiClientSecure client, const char *query){
  GraphqlQuery *graphql = (GraphqlQuery *) malloc(sizeof(GraphqlQuery));
  HTTPClient http;
  StaticJsonDocument<200> reqJson;

  syncPrintfClr("Sending HTTP POST\nrequest to\n\n%s", _ANILIST_HOST);
  http.begin(client, _ANILIST_HOST);
  http.addHeader("Content-Type", "application/json");
  reqJson["query"] = query;

  serializeJson(reqJson, graphql->req);
  Serial.printf("HTTP REQ: %s\n", graphql->req);
  graphql->status = http.POST(graphql->req);
        
  if(graphql->status > 0){
    syncPrintf("\n\nHTTP POST %d\n", graphql->status);
    strcpy(graphql->resp, http.getString().c_str());

    if(graphql->status != HTTP_CODE_OK){  
      Serial.printf("HTTP ERR: %s\n", graphql->resp);
    }
  } else{
    Serial.printf("HTTP FAILED: %s\n", http.errorToString(graphql->status).c_str());
  }
  http.end();
  reqJson.clear();
  return graphql;
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
  syncPrintfClr("SSID: %s\nIP: %s\n", _WIFI_SSID, WiFi.localIP().toString().c_str());
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
