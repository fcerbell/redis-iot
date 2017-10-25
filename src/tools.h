//
// Tooling for debugging and profiling
//

// @todo: DEBUG_DBG
// @todo: DEBUG_INF
// @todo: DEBUG_WRN
// @todo: DEBUG_CRT (with endless loop)
// @todo: DEBUG_PRINT(...) VARIADIC
// @todo: PROF_STATS in STATS_LOOP

#ifndef __TOOLS_H_
#define __TOOLS_H_

#ifdef DEBUG
#define DEBUG_PRINTLN(x) Serial.println(x); Serial.flush()
#define DEBUG_PRINT(x)   Serial.print(x); Serial.flush()
#define DEBUG_STOP while(1) yield()
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#define DEBUG_STOP
#endif

#ifdef STATS
#ifndef STATS_PERIOD
#define STATS_PERIOD 5000
#endif
#ifndef STATS_MEM
extern "C" {
#include "user_interface.h"
}
#define STATS_MEM Serial.print(", Heap: ");Serial.print(system_get_free_heap_size()); Serial.print(" bytes");
#endif
#ifndef STATS_HOOK
#define STATS_HOOK
#endif
uint16_t _loopcounter = 0;
unsigned long _lasttime = 0;
#define STATS_LOOP \
  if (millis() - _lasttime > STATS_PERIOD) { \
    Serial.print (_loopcounter *1000 / STATS_PERIOD); \
    Serial.print(" loop/second"); \
    STATS_MEM; \
    STATS_HOOK; \
    Serial.println(); \
    Serial.flush(); \
    _lasttime = millis(); \
    _loopcounter = 0; \
  } \
  _loopcounter++;
#else
#define STATS_LOOP
#endif

#ifdef PROF
#define PROF_START(x) unsigned long prof##x=micros()
#define PROF_STOP(x) Serial.print(#x); Serial.print(": "); Serial.print(micros()-prof##x); Serial.println(" µs"); Serial.flush(); prof##x=micros()
#else
#define PROF_START(x)
#define PROF_STOP(x)
#endif

#endif // __TOOLS_H_

