#ifndef _REDISCOMMAND_H_
#define _REDISCOMMAND_H_

/******************/
/* RedisCommand.h */
/******************/

#include "tools.h"

typedef struct RedisCommand_s {
  uint8_t nbStrings = 0;
  char*   szStrings = NULL;
} RedisCommand_t;

struct RedisCommand_s& rediscommand_init(struct RedisCommand_s& r);
struct RedisCommand_s& rediscommand_add(struct RedisCommand_s& r, const char* szText);
struct RedisCommand_s& rediscommand_add(struct RedisCommand_s& r, const long Number);
char* rediscommand_tochar(const struct RedisCommand_s& r);

/******************/
/* RedisCommand.c */
/******************/

struct RedisCommand_s& rediscommand_init(struct RedisCommand_s& r) {
  r.nbStrings = 0;
  if ( NULL != r.szStrings ) {
    free(r.szStrings);
    r.szStrings = NULL;
  }
  if (NULL == (r.szStrings = strdup(""))) {
    Serial.print("rediscommand_init() failed : OOM");
    DEBUG_STOP;
  };
  return r;
}

struct RedisCommand_s& rediscommand_add(struct RedisCommand_s& r, const char* szText) {
  size_t iLen = strlen(szText);
  static char szLen[10] = "";
  itoa(iLen, szLen, 10);

  // $ + TextSize + CR+LF + Text + CR+LF + Ending null
  size_t newSize = strlen(r.szStrings) + 1 + strlen(szLen) + 2 + strlen(szText) + 2 + 1;
  if (NULL == (r.szStrings = (char*)realloc(r.szStrings, newSize))) {
    Serial.println("rediscommand_add(");
    Serial.println(szText);
    Serial.println(") failed : OOM");
    while (1); // Hardware watchdog should trigger and reboot
  }
  r.szStrings = strcat(r.szStrings, "$");
  r.szStrings = strcat(r.szStrings, szLen);
  r.szStrings = strcat(r.szStrings, "\r\n");
  r.szStrings = strcat(r.szStrings, szText);
  r.szStrings = strcat(r.szStrings, "\r\n");
  r.nbStrings++;
  return r;
}

struct RedisCommand_s& rediscommand_add(struct RedisCommand_s& r, const long Number) {
  static char a[7] = "";
  ltoa(Number, a, 10);
  return rediscommand_add(r, a);
};

char* rediscommand_tochar(const struct RedisCommand_s& r) {
  // RedisCommand converted to RESP
  char* szResp;
  // nbStrings converted to text
  char szLen[10] = "";
  itoa(r.nbStrings, szLen, 10);

  // '*' + szLen + CR+LF + szStrings
  if (NULL == (szResp = (char*) malloc(1 + strlen(szLen) + 2 + strlen(r.szStrings)))) {
    Serial.println("rediscommand_tochar() failed : OOM");
    while (1); // Hardware watchdog should trigger and reboot
  }

  strcpy(szResp, "*");
  strcat(szResp, szLen);
  strcat(szResp, "\r\n");
  strcat(szResp, r.szStrings);

  return szResp;
};

#endif // _REDISCOMMAND_H_

