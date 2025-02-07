#include <ss_oled.h>

SSOLED ssoled[2];

uint8_t RDUbuffer[1024];
uint8_t LDUbuffer[1024];

void setup() {
  oledSetBackBuffer(&ssoled[0], LDUbuffer);
  oledSetBackBuffer(&ssoled[1], RDUbuffer);
  oledInit(&ssoled[1], OLED_128x64, -1, 0, 0, 0, A2, A3, -1, 400000L);
  oledInit(&ssoled[0], OLED_128x64, -1, 0, 0, 0, A4, A5, -1, 400000L);
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
}

void loop(){
  oledWriteString(&ssoled[0], 0, 1, 0, (char *)"Settings         ", FONT_NORMAL, 1, 1);
  oledSetBackBuffer(&ssoled[0], LDUbuffer);
  oledRectangle(&ssoled[0], 1, 8, 127, 63, 1, 0);
  oledWriteString(&ssoled[0], 0, 3, 2, (char *)"Default time", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[0], 0, 3, 3, (char *)"Current time", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[0], 0, 3, 4, (char *)"Misc settings", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[0], 0, 3, 5, (char *)"Firmware update", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[0], 0, 3, 6, (char *)"About", FONT_NORMAL, 0, 1);
  oledDumpBuffer(&ssoled[0], (NULL));
}