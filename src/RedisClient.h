/*****************/
/* RedisClient.h */
/*****************/

#ifndef _REDISCLIENT_H_
#define _REDISCLIENT_H_

#include "RedisCommand.h"
#include "tools.h"

typedef struct RedisClient_s {
  char* redisEndpointName = NULL;
  char* redisEndpointPass = NULL;
  uint16_t redisEndpointPort = 0;
  WiFiClient redisConnection;
  char* redisPipeline = NULL;
  uint16_t redisPipelineSize;
  uint16_t redisPipelineMaxSize;
} RedisClient_t;

struct RedisClient_s& redisclient_init(struct RedisClient_s& r, const char* endpointName, const uint16_t endpointPort);
struct RedisClient_s& redisclient_init(struct RedisClient_s& r, const char* endpointName, const uint16_t endpointPort, const char* endpointPass);
uint16_t redisclient_getpipelinemaxsize(struct RedisClient_s& r);
struct RedisClient_s& redisclient_setpipelinemaxsize(struct RedisClient_s& r, const uint16_t pipelinemaxsize);
struct RedisClient_s& redisclient_open(struct RedisClient_s& r);
struct RedisClient_s& redisclient_close(struct RedisClient_s& r);
struct RedisClient_s& redisclient_send(struct RedisClient_s& r, const struct RedisCommand_s cmd);

/*****************/
/* RedisClient.c */
/*****************/

struct RedisClient_s& redisclient_init(struct RedisClient_s& r, const char* endpointName, const uint16_t endpointPort) {
    return redisclient_init(r, endpointName, endpointPort, "");
};

struct RedisClient_s& redisclient_init(struct RedisClient_s& r, const char* endpointName, const uint16_t endpointPort, const char* endpointPass) {
  r.redisEndpointPort = endpointPort;
  if ( NULL != r.redisEndpointName )
    free(r.redisEndpointName);
  r.redisEndpointName = strdup(endpointName);
  if ( NULL != r.redisEndpointPass )
    free(r.redisEndpointPass);
  r.redisEndpointPass = strdup(endpointPass);
  if ( NULL != r.redisPipeline )
    free(r.redisPipeline);
  r.redisPipeline = strdup("");
  r.redisPipelineSize = 0;
  r.redisPipelineMaxSize = 1;
  return r;
};

uint16_t redisclient_getpipelinemaxsize(struct RedisClient_s& r) {
  return r.redisPipelineMaxSize;
};

struct RedisClient_s& redisclient_setpipelinemaxsize(struct RedisClient_s& r, const uint16_t pipelinemaxsize) {
  r.redisPipelineMaxSize = pipelinemaxsize;
  return r;
};

struct RedisClient_s& redisclient_open(struct RedisClient_s& r) {
  // Connect to the endpoint pool IPs
  DEBUG_PRINT("Resolving '");
  DEBUG_PRINT(r.redisEndpointName);
  DEBUG_PRINT("' IP : ");
  // no IP global variable to force a resolution each time from the DNS IP pool
  IPAddress redisEndpointIP;
  WiFi.hostByName(r.redisEndpointName, redisEndpointIP);
  DEBUG_PRINTLN(redisEndpointIP);
  DEBUG_PRINT("Connecting to ");
  DEBUG_PRINT(redisEndpointIP);
  DEBUG_PRINT(":");
  DEBUG_PRINT(r.redisEndpointPort);
  DEBUG_PRINT("... ");
  if (!r.redisConnection.connect(redisEndpointIP, r.redisEndpointPort)) {
    DEBUG_PRINTLN(" Failed");
    return r;
  } else {
    DEBUG_PRINT(" Succeed...");
  }
  if (strlen(r.redisEndpointPass)) {
    DEBUG_PRINT(" AUTHenticating...");
    RedisCommand_t auth;
    char* szAuth;
    rediscommand_init(auth);
    rediscommand_add(auth,"auth");
    rediscommand_add(auth,r.redisEndpointPass);
    szAuth = rediscommand_tochar(auth);
    r.redisConnection.print(szAuth);
    free(szAuth);
    // workaround waiting for reply parsing functions
    while (r.redisConnection.read() != -1);
  }
  DEBUG_PRINTLN(" OK");
  return r;
};

// Push in the pipeline
struct RedisClient_s& redisclient_send(struct RedisClient_s& r, const struct RedisCommand_s cmd) {
  char* szResp = rediscommand_tochar(cmd);
  if (NULL == (r.redisPipeline = (char*)realloc(r.redisPipeline, strlen(r.redisPipeline) + strlen(szResp) + 1))) {
    Serial.println("redisclient_send failed : OOM");
    DEBUG_STOP;
  }
  strcat(r.redisPipeline, szResp);
  free(szResp);
  r.redisPipelineSize++;
  if (r.redisPipelineSize >= r.redisPipelineMaxSize) {
    while (!r.redisConnection.connected())
      redisclient_open(r);
    digitalWrite(LED_BUILTIN, LOW);
    r.redisConnection.print(r.redisPipeline);
    digitalWrite(LED_BUILTIN, HIGH);
    free(r.redisPipeline);
    r.redisPipeline = strdup("");
    r.redisPipelineSize = 0;
    // workaround waiting for reply parsing functions
    while (r.redisConnection.read() != -1);
  }
  return r;
};

struct RedisClient_s& redisclient_close(struct RedisClient_s& r) {
  if (!r.redisConnection.connected()) {
    DEBUG_PRINTLN("RedisClientESP::close(): Already closed.");
    return r;
  }
  r.redisConnection.stop();
  DEBUG_PRINT("Connection ");
  DEBUG_PRINT(r.redisEndpointName);
  DEBUG_PRINT(":");
  DEBUG_PRINT(r.redisEndpointPort);
  DEBUG_PRINTLN(" closed");
  return r;
};


#endif // _REDISCLIENT_H_

