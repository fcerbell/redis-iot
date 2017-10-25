// FEATURE: Use current value if new value is empty
// BUGFIX: Disconnect ESP's Wifi before testing
// FEATURE: Hide password values in the main menu
// FEATURE: Display password current value only when asking for a new value
// FEATURE: Save Wifi and Redis credentials in EEPROM (impacts other application)
// FEATURE: Format EEPROM with default values if no magic number (version) found
// FEATURE: Overloads default values with EEPROM values
// BUGFIX: Add Yield call to avoid keyword search timeout
// FEATURE: Simplified eeprom.h and moved to library for useage in RedisClient

#include "eeprom.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define MENUCMD(NAME,LABEL,VALUE, SAVEENTRY,SHOWVALUE)  { #NAME, LABEL, strdup(VALUE), NAME##_func, SAVEENTRY,SHOWVALUE }
#define MENUFCT(NAME) void NAME##_func(void)

typedef void (*CommandFunc_t)(void);

typedef struct Command_s {
  char* name;
  char* label;
  char* value;
  CommandFunc_t func;
  bool saveEntry;
  bool showValue;
} Command_t;

CommandFunc_t CurrentState;

MENUFCT(menu);
MENUFCT(wifissid);
MENUFCT(wifipass);
MENUFCT(wifitest);
MENUFCT(redishost);
MENUFCT(redisport);
MENUFCT(redispass);
MENUFCT(redistest);
MENUFCT(firstname);
MENUFCT(lastname);
MENUFCT(email);
MENUFCT(screenname);
MENUFCT(githubuser);
MENUFCT(githubproject);
MENUFCT(githubtest);
MENUFCT(twitteruser);
MENUFCT(twittertest);
MENUFCT(testADC);
MENUFCT(test);
MENUFCT(save);
MENUFCT(resetEEPROM);

Command_t commands[] =
{
  MENUCMD (menu, "Refresh menu", "", false, true),
  MENUCMD (wifissid, "WIFI network SSID (to check connection)", "MyWifiNetwork", true, true),
  MENUCMD (wifipass, "WIFI password (to check connection)", "MyWifiPassword", true, false),
  //  MENUCMD (wifitest, "WIFI connection test", "",false,false),
  MENUCMD (redishost, "Redis cluster hostname or IP (to check connection and register)", "redis-14658.demo.francois.demo-rlec.redislabs.com", true, true),
  MENUCMD (redisport, "Redis cluster port (to check connection and register)", "14658", true, true),
  MENUCMD (redispass, "Redis db password (to check connection and register)", "iot", true, false),
  //  MENUCMD (redistest, "Redis connection test (needs a working WIFI)", "",false,false),
  MENUCMD (firstname, "Firstname (for pretty display)", "François", false, true),
  MENUCMD (lastname, "Lastname (for pretty display)", "Cerbelle", false, true),
  MENUCMD (email, "Email (to send you the workshop materials)", "francois@redislabs.com", false, true),
  MENUCMD (screenname, "Screenname (to be displayed on video projector)", "Fanfan", false, true),
  MENUCMD (githubuser, "GitHub account (to check the project creation)", "fcerbell", false, true),
  MENUCMD (githubproject, "GitHub Project (to check the project creation)", "iot-redis-esp", false, true),
  //  MENUCMD (githubtest, "GitHub account and project test (needs a working WIFI)", "",false,false),
  MENUCMD (twitteruser, "Twitter account (to check the twits)", "fcerbelle", false, true),
  //  MENUCMD (twittertest, "Twitter account test (needs a working WIFI)", "",false,false),
  MENUCMD (testADC, "Test the sensor", "", false, false),
  MENUCMD (test, "Check all the informations without registration", "", false, false),
  MENUCMD (save, "Register your details and the device MAC address in the cluster", "", false, false),
  MENUCMD (resetEEPROM, "Reset all information to default values", "", false, false),
  0
};

Command_t *getCommandByName(const char* name) {
  Command_t *it = commands;
  while ((*(char*)it != 0) && (strcmp(name, it->name))) it++;
  return it;
};

// Used when converting to integer
char* tmpAnswer = strdup("");
bool getAnswer(char* &value) {
  static String answer = "";
  if (Serial.available() > 0) {
    answer = Serial.readString();
    answer.replace("\n", "");
    answer.replace("\r", "");
    while (Serial.available() > 0) Serial.read();
    Serial.print("<");
    Serial.print(answer);
    Serial.println(">");
    if (answer.length() > 0) {
      free(value);
      value = strdup(answer.c_str());
    }
    return (true);
  }
  return (false);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.println();
  Serial.println("Serial initialized");
  Serial.print("WiFi mode setup...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("OK");
  delay(100);
  loadFromEEPROM();
  CurrentState = menu_func;
}

void loop() {
  CurrentState();
}
