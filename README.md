# Anilist-ESP8266

A small Friday night project to use an ESP8266 to query Anilist GraphQL API and display profile data on a mini OLED.


## Purpose
I haven't touched any Arduino stuff in a couple years, so I figured I needed a fun refresher before tackling a different project. This small project was used to learn more about [PlatformIO](https://platformio.org/) and HTTP requests on ESP8266.
I used the Anilist API for this because I've already used it in a couple other weekend projects and it is super easy to mess around with.


## Configuration
My small brain couldn't figure out loading secrets with [PlatformIO advanced scripting](https://docs.platformio.org/en/latest/projectconf/advanced_scripting.html#construction-environments), so I just setup a separate header file called **anilist8266_config.h.h**.
Its still plaintext credentials though...I think it works good enough for a messaround project.
```c
/* include/anilist8266_config.h */

#ifndef ANILIST8266_CONFIG_H
#define ANILIST8266_CONFIG_H

const char *_ANILIST_USER = "user";

const char *_WIFI_SSID = "SSID";
const char *_WIFI_PWD  = "PASSWORD"; 

#endif
```


## Screenshots
TODO:



## References
* [Arduino ESP8266HTTPClient Examples](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266HTTPClient/examples)
* [Arduino JSON Examples](https://arduinojson.org/v6/example/)
* [Arduino JSON Assistant](https://arduinojson.org/v6/assistant/)
* [Introduction to PlatformIO](https://www.youtube.com/watch?v=0poh_2rBq7E)
* [PlatformIO](https://platformio.org/)
* [PlatformIO IDE Extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)
* [Anilist Interactive GraphQL Tool](https://anilist.co/graphiql)