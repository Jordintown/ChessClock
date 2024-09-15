#include <SPI.h>
#include <Wire.h>
#include <ss_oled.h>

#define GROVE_SDA_PIN A2
#define GROVE_SCL_PIN A3
#define SDA_PIN A4
#define SCL_PIN A5
#define RESET_PIN -1
#define OLED_ADDR -1
#define FLIP180 0
#define INVERT 0
#define USE_HW_I2C 0
#define MY_OLED1 OLED_128x64
#define MY_OLED2 OLED_128x64

SSOLED ssoled[2];

unsigned long clock, segundos, ultimoclock;
unsigned long segundos1 = 10, segundos2 = 10;
int jugador1 = 0;
int moves1 = 0, moves2 = 0;
byte balancinstat, balancinstat2;
int player = 0;

void setup() {
  char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D"};
  int rc;
  rc = oledInit(&ssoled[0], MY_OLED1, OLED_ADDR, FLIP180, INVERT, 1, SDA_PIN, SCL_PIN, RESET_PIN, 400000L);
  rc = oledInit(&ssoled[1], MY_OLED2, OLED_ADDR, FLIP180, INVERT, 0, GROVE_SDA_PIN, GROVE_SCL_PIN, RESET_PIN, 400000L);
  oledFill(&ssoled[0], 0, 1);
  oledWriteString(&ssoled[0], 0, 0, 0, (char *)"DME Chess Clock", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[0], 0, 10, 3, (char *)"INIT", FONT_STRETCHED, 0, 1);
  oledFill(&ssoled[1], 0, 1);
  oledWriteString(&ssoled[1], 0, 60, 0, (char *)"v0.0.0A", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[1], 0, 10, 3, (char *)"INIT", FONT_STRETCHED, 0, 1);

  Serial.begin(9600);
  ultimoclock = 0;

  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
}

void loop() {
  clock = millis();
  segundos = clock / 1000;

  if (clock > (ultimoclock + 999)) {
    ultimoclock = clock;

    if (player == 0) {  // Jugador 1
      segundos1--;
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);

      oledWriteString(&ssoled[0], 0, 0, 0, (char *)"Moves: 0", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled[0], 0, 85, 0, (char *)"min/sec", FONT_SMALL, 0, 1);
      oledWriteString(&ssoled[0], 0, 20, 3, (char *)"00:00", FONT_STRETCHED, 0, 1);

      // Manejar el tiempo agotado
      if (segundos1 < 1) {
        digitalWrite(10, HIGH);
        delay(500);
      }

    } else {  // Jugador 2
      segundos2--;
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);

      oledWriteString(&ssoled[1], 0, 0, 0, (char *)"Moves: 0", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled[1], 0, 85, 0, (char *)"min/sec", FONT_SMALL, 0, 1);
      oledWriteString(&ssoled[1], 0, 20, 3, (char *)"00:00", FONT_STRETCHED, 0, 1);

      // Manejar el tiempo agotado
      if (segundos2 < 1) {
        digitalWrite(10, HIGH);
        delay(500);
      }
    }
  }

  // Detectar cambio de jugador
  balancinstat = digitalRead(2);
  balancinstat2 = digitalRead(3);

  if (balancinstat == LOW) {  // Botón del Jugador 1 presionado
    player = 1; // Cambia al Jugador 2
    moves1++;
    Serial.println("Cambio a Jugador 2");

    // Espera hasta que el botón se suelte
    while (digitalRead(2) == LOW) {
      // Aquí puedes realizar otras tareas si es necesario
    }
  }

  if (balancinstat2 == LOW) {  // Botón del Jugador 2 presionado
    player = 0; // Cambia al Jugador 1
    moves2++;
    Serial.println("Cambio a Jugador 1");

    // Espera hasta que el botón se suelte
    while (digitalRead(3) == LOW) {
      // Aquí puedes realizar otras tareas si es necesario
    }
  }
}
