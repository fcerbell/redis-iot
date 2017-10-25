#include "RedisClient.h"

char* magicNumber = "redis1";

RedisClient_t client;
RedisCommand_t cmd;

void saveToEEPROM() {
  int addr = 0;

  EEPROM.begin(512);
  Serial.print("Writing config to EEPROM...");
  addr = writeStringToEEPROM(0, magicNumber);
  for (Command_t *it = commands; * (char*)it != 0; it++) {
    if (it->saveEntry)
      addr = writeStringToEEPROM(addr, it->value);
  }
  EEPROM.commit();
  EEPROM.end();
  Serial.println("OK");
}

void loadFromEEPROM() {
  int addr;
  char* value = NULL;

  EEPROM.begin(512);
  addr = readStringFromEEPROM(0, value, strlen(magicNumber));
  if (0 != strcmp(magicNumber, value)) {
    Serial.println("Reinitializing EEPROM with current values");
    saveToEEPROM();
    return;
  }

  Serial.print("Reading config from EEPROM...");
  for (Command_t *it = commands; * (char*)it != 0; it++) {
    if (it->saveEntry)
      addr = readStringFromEEPROM(addr, it->value, 50);
  }
  Serial.println("OK");
  EEPROM.end();
}

MENUFCT(resetEEPROM) {
  EEPROM.begin(512);
  clearEEPROM();
  EEPROM.commit();
  EEPROM.end();
  Serial.println("If the device does not reboot automatically, press the reset button");
  while (1);
}

MENUFCT(save) {
  test_func(); if (CurrentState != menu_func) return;
  Serial.println();
  upload_func();
  // Endless pause without WDT
  while (1) yield();
}

MENUFCT(upload) {
  Serial.print("Registration in the cluster...");

  redisclient_init(client, getCommandByName("redishost")->value, atoi(getCommandByName("redisport")->value), getCommandByName("redispass")->value);
  redisclient_setpipelinemaxsize(client, 1);
  char key[20] = "r:";
  strcat(key, WiFi.macAddress().c_str());

  // Delete an eventually existing key to avoid a TYPE conflict
  rediscommand_init(cmd);
  rediscommand_add(cmd, "DEL");
  rediscommand_add(cmd, key);
  redisclient_send(client, cmd);

  rediscommand_init(cmd);
  rediscommand_add(cmd, "HMSET");
  rediscommand_add(cmd, key);
  rediscommand_add(cmd, "macaddr");  rediscommand_add(cmd, WiFi.macAddress().c_str());
  rediscommand_add(cmd, "firstname");  rediscommand_add(cmd, getCommandByName("firstname")->value);
  rediscommand_add(cmd, "lastname");  rediscommand_add(cmd, getCommandByName("lastname")->value);
  rediscommand_add(cmd, "email");  rediscommand_add(cmd, getCommandByName("email")->value);
  rediscommand_add(cmd, "screenname");  rediscommand_add(cmd, getCommandByName("screenname")->value);
  rediscommand_add(cmd, "githubuser");  rediscommand_add(cmd, getCommandByName("githubuser")->value);
  rediscommand_add(cmd, "githubproject");  rediscommand_add(cmd, getCommandByName("githubproject")->value);
  rediscommand_add(cmd, "twitteruser");  rediscommand_add(cmd, getCommandByName("twitteruser")->value);
  redisclient_send(client, cmd);

  rediscommand_init(cmd);
  rediscommand_add(cmd, "SADD");
  rediscommand_add(cmd, "devices");
  rediscommand_add(cmd, WiFi.macAddress().c_str());
  redisclient_send(client, cmd);

  rediscommand_init(cmd);
  rediscommand_add(cmd, "PUBLISH");
  rediscommand_add(cmd, "refreshdevices");
  rediscommand_add(cmd, WiFi.macAddress().c_str());
  redisclient_send(client, cmd);

  Serial.println("OK");
  CurrentState = menu_func;
}

MENUFCT(test) {
  Serial.println();
  saveToEEPROM();
  Serial.println("Testing...");
  wifitest_func(); if (CurrentState != menu_func) return;
  redistest_func(); if (CurrentState != menu_func) return;
  githubtest_func(); if (CurrentState != menu_func) return;
  twittertest_func(); if (CurrentState != menu_func) return;
  return;
}

