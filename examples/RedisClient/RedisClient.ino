//
// IoT Redis Workshop
// F.Cerbelle
//
// Goal : Understand and use the RESP Protocol and PubSub
//        for realtime sensor measure from a microcontroller
//        to a web dashboard
//
// Base file :
// - Serial console initialized with DEBUG, STATS and PROFILING
// - Disabled WiFi debug on Serial
// - Wifi network configuration and connection
// - LED configuration and blink on stats
// - Wifi connection sample with send and receive
// - Sensor read every 5 seconds
//
// Notice that everything is done to be non-blocking,
// the main loop has to run as fast as possible
//

// Wifi headers for ESP8266
#include <ESP8266WiFi.h>

// Configuration
#define DEBUG
#define PROF
#define STATS
//#define STATS_MEM
#define STATS_HOOK digitalWrite(LED_BUILTIN,((digitalRead(LED_BUILTIN)==HIGH)?LOW:HIGH))
#define STATS_PERIOD 1000
#include "tools.h"

// Read Wifi SSID/Password, and Redis Host/Port/Pass from EEPROM
// in wifi_ssid, wifi_pass, redis_host, redis_port and redis_pass (all char*)
#define magicNumber "redis1"
#include "redisiot.h"

// Last print timestamp
unsigned long lastSensorRead = 0;

/********/
/* Main */
/********/
void setup() {
  Serial.begin(115200);
  //  Serial.setDebugOutput(true);
  while (!Serial);
  DEBUG_PRINTLN("Serial initialized.");

  // Read Wifi and Redis cluster credentials from EEPROM (stored by the registration)
  LoadConfig();

  // WIFI connection stuff
  DEBUG_PRINT("Connecting to ");
  DEBUG_PRINT(wifi_ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN("");
  DEBUG_PRINT("WiFi (");
  DEBUG_PRINT(WiFi.macAddress());
  DEBUG_PRINT(") connected with IP ");
  DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINT("DNS0: ");
  DEBUG_PRINTLN(WiFi.dnsIP(0));
  DEBUG_PRINT("DNS1: ");
  DEBUG_PRINTLN(WiFi.dnsIP(1));

  // 5 quick LED blinks
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
}

void loop() {
  STATS_LOOP

  if ((millis() - lastSensorRead) > 5000) {
    PROF_START(SensorRead);
    Serial.print("Sensor value (0-1024): ");
    Serial.println(analogRead(0));
    PROF_STOP(SensorRead);
    lastSensorRead = millis();
  }
}
