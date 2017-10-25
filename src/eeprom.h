#ifndef __eeprom_h__
#define __eeprom_h__

#include <EEPROM.h>

void clearEEPROM() {
  Serial.print("Erasing EEPROM...");
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
  Serial.println("OK");
}

uint8_t writeStringToEEPROM(uint8_t offset, char* &value) {
  int addr;
  if ((offset + 1 + strlen(value)) > 512) {
    Serial.println("Value end outside of flash boundaries...");
    Serial.println("Check your code, empty string saved instead");
    EEPROM.write(offset++, 0);
    return offset;
  }
  EEPROM.write(offset, strlen(value));
  for (addr = offset + 1; addr <= offset + strlen(value); addr++)
    EEPROM.write(addr, value[addr - offset - 1]);
  return (offset + 1 + strlen(value));
}

uint8_t readStringFromEEPROM(uint8_t offset, char* &value, uint8_t maxLength) {
  int addr;
  uint8_t len = EEPROM.read(offset);
  if ((offset + 1 + len) > 512) {
    Serial.println("Value end outside of flash boundaries...");
    Serial.println("Reformat/Reset saved values");
    Serial.println("Truncated value");
    len = 512 - 1 - offset;
  }
  if (len > maxLength) {
    Serial.println("Value size bigger than max allowed size");
    Serial.println("Reformat/Reset saved values");
    Serial.println("Truncated value");
    len = maxLength;
  }
  value = (char*)realloc(value, len + 1);
  for (addr = offset + 1; addr <= offset + len; addr++)
    value[addr - offset - 1] = EEPROM.read(addr);
  value[len] = 0;
  return (offset + 1 + len);
}

#endif // __eeprom_h__

