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
#include <RedisCommand.h>

// Last print timestamp
unsigned long lastSensorRead = 0;

WiFiClient redisConnection;
IPAddress redisIP;
RedisCommand_t redisCommand;
char* szRESP;

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

  if (!redisConnection.connected () ) {
    DEBUG_PRINT ("Opening connection to ");
    DEBUG_PRINT ( redis_host );
    DEBUG_PRINT ("(") ;
    WiFi.hostByName ( redis_host , redisIP ) ;
    DEBUG_PRINT ( redisIP ) ;
    DEBUG_PRINT ("):");
    int redisPort = atoi(redis_port);
    DEBUG_PRINT ( redis_port );
    DEBUG_PRINT ("...");
    if (!redisConnection.connect(redisIP, redisPort)) {
      DEBUG_PRINTLN ("Failed, press reset") ;
      while (1) yield();
    } else {
      DEBUG_PRINTLN ("Succeed");
    }

    // Send AUTH in RESP
    rediscommand_init(redisCommand);
    rediscommand_add(redisCommand, "AUTH");
    rediscommand_add(redisCommand, redis_pass);
    szRESP = rediscommand_tochar(redisCommand);
    redisConnection.print(szRESP);
    free (szRESP) ;

    // Expect a reply and busy ( bad ) wait for it
    while ( redisConnection.available () == 0) ;

    // Output to the console all the received bytes as chars
    while ( redisConnection.available () != 0)
      Serial.print (( char ) redisConnection.read () );
  }

  // Add your sensor value to the begining of the device 's value list
  rediscommand_init(redisCommand);
  rediscommand_add(redisCommand, "LPUSH");
  rediscommand_add(redisCommand, (String("v:") + WiFi.macAddress()).c_str());
  rediscommand_add(redisCommand, analogRead(0));
  szRESP = rediscommand_tochar(redisCommand);
  redisConnection.print(szRESP);
  free (szRESP) ;

  // Expect a reply and busy ( bad ) wait for it
  while ( redisConnection.available () == 0) ;

  // Output to the console all the received bytes as chars
  while ( redisConnection.available () != 0)
    Serial.print (( char ) redisConnection.read () );

  // Notify the web dashboard to update the values for your device
  rediscommand_init(redisCommand);
  rediscommand_add(redisCommand, "PUBLISH");
  rediscommand_add(redisCommand, "refreshvalues");
  rediscommand_add(redisCommand, WiFi.macAddress().c_str());
  szRESP = rediscommand_tochar(redisCommand);
  redisConnection.print(szRESP);
  free (szRESP) ;

  // Expect a reply and busy ( bad ) wait for it
  while (redisConnection.available() == 0) ;

  // Output to the console all the received bytes as chars
  while ( redisConnection.available() != 0)
    redisConnection.read() ;

  if ((millis() - lastSensorRead) > 5000) {
    PROF_START(SensorRead);
    Serial.print("Sensor value (0-1024): ");
    Serial.println(analogRead(0));
    PROF_STOP(SensorRead);
    lastSensorRead = millis();
  }
}
