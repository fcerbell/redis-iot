#ifndef __redisiot_h__
#define __redisiot_h__

#include "eeprom.h"

//  your network SSID (name)
char* wifi_ssid;
char* wifi_pass;

// Redis IoT cluster
char* redis_host;
char* redis_port;
char* redis_pass;

void LoadConfig() {
  int addr;
  char* value = NULL;
  EEPROM.begin(512);
  addr = readStringFromEEPROM(0, value, strlen(magicNumber));
  if (0 != strcmp(magicNumber, value)) {
    Serial.println("Bad EEPROM magic number");
    while (1) yield();
  }
  DEBUG_PRINT("Reading config from EEPROM...");
  addr = readStringFromEEPROM(addr, wifi_ssid, 50);
  addr = readStringFromEEPROM(addr, wifi_pass, 50);
  addr = readStringFromEEPROM(addr, redis_host, 50);
  addr = readStringFromEEPROM(addr, redis_port, 50);
  addr = readStringFromEEPROM(addr, redis_pass, 50);
  EEPROM.end();
  DEBUG_PRINTLN("OK");
  DEBUG_PRINT("wifi_ssid: "); DEBUG_PRINTLN(wifi_ssid);
  DEBUG_PRINT("redis_host: "); DEBUG_PRINTLN(redis_host);
  DEBUG_PRINT("redis_port: "); DEBUG_PRINTLN(redis_port);
  DEBUG_PRINT("redis_pass: "); DEBUG_PRINTLN(redis_pass);
  DEBUG_PRINTLN();
}

#endif // __redisiot_h__

