#define KEYWORD(w,g) { strdup(w), g, 0 }
#define KW_PRODUCT(w) KEYWORD(w,0)
#define KW_COMPANY(w) KEYWORD(w,1)
#define KW_SPONSOR(w) KEYWORD(w,2)
#define KW_TOPIC(w) KEYWORD(w,3)
#define KW_BONUS(w) KEYWORD(w,4)

typedef struct Keyword_s {
  char* keyword;
  uint8_t group;
  uint16_t count;
} Keyword_t;

uint16_t groups[5];

Keyword_t keywords[] =
{
  KW_PRODUCT("redis"),
  KW_COMPANY("redislabs"),
  //  KW_SPONSOR("soat"),
  KW_SPONSOR("zenika"),
  //  KW_SPONSOR("etincelle"),
  //  KW_SPONSOR("cordee"),
  //  KW_SPONSOR("wagon"),
  //  KW_SPONSOR("altnet"),
  //  KW_SPONSOR("librepc"),
  KW_TOPIC("iot"),
  KW_TOPIC("embedded"),
  KW_TOPIC("arduino"),
  KW_TOPIC("esp"),
  KW_TOPIC("esp8266"),
  KW_TOPIC("esp-12e"),
  KW_TOPIC("esp-12f"),
  KW_BONUS("meetup"),
  KW_BONUS("nosql"),
  KW_BONUS("opensource"),
  KW_BONUS("#"),
  0
};

#define BUFFSIZE 128
#define OVERLAP 8
void kw_parse(HTTPClient &http) {
  Keyword_t *it;
  int len = http.getSize();
  uint8_t buff[BUFFSIZE] = { 0 };
  for (uint8_t i = 0; i < 5; i++) groups[i] = 0;
  for (it = keywords; * (char*)it != 0; it++) it->count = 0;
  WiFiClient * stream = http.getStreamPtr();
  while (http.connected() && (len > 0 || len == -1)) {
    size_t size = stream->available();
    if (size) {
      strncpy((char*)buff, (char*)&buff[BUFFSIZE - OVERLAP], OVERLAP);
      int c = stream->readBytes(&buff[OVERLAP], ((size > (BUFFSIZE - OVERLAP)) ? (BUFFSIZE - OVERLAP) : size));
      for (int i = 0; i < BUFFSIZE; i++) buff[i] = (uint8_t)tolower((char)(buff[i]));
      for (it = keywords; * (char*)it != 0; it++)
        if (strstr((char*)buff, it->keyword) != NULL) {
          it->count++;
          groups[it->group]++;
        }
      if (len > 0) len -= c;
    }
    delay(1);
    yield();
  }
  http.end();
}

void kw_show() {
  Keyword_t *it;
  Serial.print("[");
  for (it = keywords; * (char*)it != 0; it++) {
    Serial.print(it->keyword);
    Serial.print(":");
    Serial.print(it->count);
    Serial.print(((* (char*)(it + 1) != 0) ? "," : "]"));
  }
  Serial.println();
}

void kg_show() {
  Serial.println("[Products,Company,Sponsors,Topics,Bonus]");
  Serial.print("[");
  for (uint8_t i = 0; i < 5; i++) {
    Serial.print(groups[i]);
    Serial.print((i < 4 ? "," : "]"));
  }
  Serial.println();
}

