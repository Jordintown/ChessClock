#include <ss_oled.h>
#include <math.h>

// Configuración de pines y pantalla
#define GROVE_SDA_PIN 32
#define GROVE_SCL_PIN 26
#define RESET_PIN -1
#define OLED_ADDR -1
#define FLIP180 0
#define INVERT 0
#define USE_HW_I2C 1
#define MY_OLED OLED_128x64

uint8_t ucBackBuffer[1024];
SSOLED ssoled[2];

void setup() {
  int inutil;
  inutil = oledInit(&ssoled[0], OLED_128x64, -1, 0, 0, 1, A4, A5, -1, 400000L);
  inutil = oledInit(&ssoled[1], OLED_128x64, -1, 0, 0, 0, A2, A3, -1, 400000L);
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
  oledWriteString(&ssoled[0], 0, 0, 3, (char *)"Self test in progress", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[0], 0, 20, 4, (char *)"(Max. 3 seconds)", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[1], 0, 0, 3, (char *)"Self test in progress", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[1], 0, 20, 4, (char *)"(Max. 3 seconds)", FONT_SMALL, 0, 1);
  oledFill(&ssoled[0], 0, 1);
  //oledFill(&ssoled[1], 0, 1);
  oledSetBackBuffer(&ssoled[0], ucBackBuffer);
  oledSetBackBuffer(&ssoled[1], ucBackBuffer);
}

void dibujaBandera(SSOLED &display){
  oledRectangle(&display, 10, 15, 30, 25, 1, 1);
  oledEllipse(&display, 16, 15, 4, 2, 0, 1);
  oledEllipse(&display, 25, 25, 4, 2, 0, 1);
  oledEllipse(&display, 25, 15, 4, 2, 1, 1);
  oledEllipse(&display, 16, 25, 4, 2, 1, 1);
  oledRectangle(&display, 10, 15, 11, 45, 1, 1);
  oledDumpBuffer(&display, ucBackBuffer);
}

void dibujaReloj(SSOLED &display){
  oledRectangle(&display, 25, 5, 35, 6, 1, 1);
  oledRectangle(&display, 29, 5, 31, 12, 1, 0);
  oledRectangle(&display, 30, 37, 30, 18, 1, 1);
  oledEllipse(&display, 30, 37, 25, 25, 1, 0);
  oledDumpBuffer(&display, ucBackBuffer);
}
void dibujaLogo(){
  oledRectangle(&ssoled[0], 25, 5, 35, 6, 1, 1);
  oledRectangle(&ssoled[0], 29, 5, 31, 12, 1, 0);
  oledRectangle(&ssoled[0], 30, 37, 30, 18, 1, 1);
  oledEllipse(&ssoled[0], 30, 37, 25, 25, 1, 0);
  oledDumpBuffer(&ssoled[0], ucBackBuffer);
}

void drawRectangleWithHollowCenter(SSOLED &display, int x, int y, int width, int height) {
  // Dibuja un rectángulo lleno (el contorno)
  oledRectangle(&display, x, y, x + width - 1, y + height - 1, 1, 1);
  
  // "Vacia" el centro del rectángulo con un rectángulo vacío (sin relleno)
  oledRectangle(&display, x + 5, y + 5, x + width - 6, y + height - 6, 0, 0); // Rectángulo vacío (sin contorno ni relleno)
  
  oledDumpBuffer(&display, NULL); // Actualiza la pantalla
}

void loop() {
  /*
  dibujaBandera(ssoled[1]);
  dibujaBandera(ssoled[0]);
  */
  
  dibujaLogo();
  //dibujaReloj(ssoled[0]);
  
}
