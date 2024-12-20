#include <ss_oled.h>
#include "config.h"
#include "conversions.h"
uint8_t LDUbuffer[1024];
uint8_t RDUbuffer[1024];
SSOLED ssoled[2];

void setup() {
  pinMode(5, INPUT_PULLUP);
  oledInit(&ssoled[0], OLED_128x64, -1, 0, 0, 1, A5, A4, -1, 400000L);
  oledInit(&ssoled[1], OLED_128x64, -1, 0, 0, 0, A2, A3, -1, 400000L);
  oledWriteString(&ssoled[0], 0, 0, 3, (char *)("Self test in progress"), FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[0], 0, 20, 4, (char *)("(Max. 3 seconds)"), FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[1], 0, 0, 3, (char *)("Self test in progress"), FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[1], 0, 20, 4, (char *)("(Max. 3 seconds)"), FONT_SMALL, 0, 1);
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
  oledSetBackBuffer(&ssoled[0], LDUbuffer);
  oledSetBackBuffer(&ssoled[1], RDUbuffer);
}

void error( char* title, char* l1, char* l2, char* l3, char* code, bool skippable){
    char buffer[16];
  if (skippable==true){
    oledWriteString(&ssoled[0], 0, 7, 1, (char *)title, FONT_NORMAL, 1, 0);
    oledWriteString(&ssoled[0], 0, 10, 3, (char *)l1, FONT_SMALL, 0, 0);
    oledWriteString(&ssoled[0], 0, 10, 4, (char *)l2, FONT_SMALL, 0, 0);
    oledWriteString(&ssoled[0], 0, 10, 5, (char *)l3, FONT_SMALL, 0, 0);
    oledRectangle(&ssoled[0], 6, 7, 124, 16, 1, 0);
    oledRectangle(&ssoled[0], 6, 7, 124, 62, 1, 0);
    oledDumpBuffer(&ssoled[0], (LDUbuffer));
    //
    oledWriteString(&ssoled[1], 0, 7, 1, (char *)"CLOCK DATA", FONT_NORMAL, 1, 0);
    oledWriteString(&ssoled[1], 0, 10, 3, (char *)("FIRMWARE: v"FIRM_VER), FONT_SMALL, 0, 0);
    sprintf(buffer, "ERR: %s", code);
    oledWriteString(&ssoled[1], 0, 10, 4, (char *)buffer, FONT_SMALL, 0, 0);
    oledWriteString(&ssoled[1], 0, 10, 5, (char *)"id: 8865", FONT_SMALL, 0, 0);
    oledRectangle(&ssoled[1], 6, 7, 124, 16, 1, 0);
    oledRectangle(&ssoled[1], 6, 7, 124, 62, 1, 0);
    oledRectangle(&ssoled[1], 6, 55, 124, 62, 1, 0);
    oledWriteString(&ssoled[1], 0, 7, 7, (char *)"PAUSE: continue", FONT_NORMAL, 1, 0);
    oledDumpBuffer(&ssoled[1], (RDUbuffer));
    while (digitalRead(5)==HIGH){}
  } else {
    oledWriteString(&ssoled[0], 0, 7, 1, (char *)title, FONT_NORMAL, 1, 0);
    oledWriteString(&ssoled[0], 0, 10, 3, (char *)l1, FONT_SMALL, 0, 0);
    oledWriteString(&ssoled[0], 0, 10, 4, (char *)l2, FONT_SMALL, 0, 0);
    oledWriteString(&ssoled[0], 0, 10, 5, (char *)l3, FONT_SMALL, 0, 0);
    oledRectangle(&ssoled[0], 6, 7, 124, 16, 1, 0);
    oledRectangle(&ssoled[0], 6, 7, 124, 62, 1, 0);
    oledDumpBuffer(&ssoled[0], (LDUbuffer));
    //
    oledWriteString(&ssoled[1], 0, 7, 1, (char *)"CLOCK DATA", FONT_NORMAL, 1, 0);
    oledWriteString(&ssoled[1], 0, 10, 3, (char *)("FIRMWARE: v"FIRM_VER), FONT_SMALL, 0, 0);
    sprintf(buffer, "ERR: %s", code);
    oledWriteString(&ssoled[1], 0, 10, 4, (char *)buffer, FONT_SMALL, 0, 0);
    oledRectangle(&ssoled[1], 6, 7, 124, 16, 1, 0);
    oledRectangle(&ssoled[1], 6, 7, 124, 62, 1, 0);
    oledDumpBuffer(&ssoled[1], (RDUbuffer));
    while (1){}
  }
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
}

void loop(){
  error("INVALID LANGPAK", "Alqag is such a bix", "that the EN_US", "LANGPAK is unusable", "INV_VAL", false);
}