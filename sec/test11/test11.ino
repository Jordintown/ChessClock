#include <ss_oled.h>

unsigned long reloj;
SSOLED ssoled[2];

void setup(){
  oledInit(&ssoled[0], OLED_128x64, -1, 0, 0, 0, A2, A3, -1, 400000L);
  oledInit(&ssoled[1], OLED_128x64, -1, 0, 0, 0, A4, A5, -1, 400000L);
}

char* intToChar(unsigned long num) {
    // Calcula el espacio necesario (incluye el carácter nulo)
    char* str = (char*)malloc(12);  // Suficiente para un int de 32 bits (+ signo y null)
    if (str != NULL) {
        sprintf(str, "%d", num);  // Convierte el int a char*
    }
    return str;
}

void loop(){
  reloj=millis();
  oledWriteString(&ssoled[0], 0, 0, 0, intToChar(reloj), FONT_SMALL, 0, 1);
  reloj=millis();
  oledWriteString(&ssoled[0], 0, 0, 1, intToChar(reloj), FONT_SMALL, 0, 1);
  reloj=millis();
  oledWriteString(&ssoled[1], 0, 0, 0, intToChar(reloj), FONT_SMALL, 0, 1);
  reloj=millis();
  oledWriteString(&ssoled[1], 0, 0, 1, intToChar(reloj), FONT_SMALL, 0, 1);
}