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
#define PAUSE 3
#define TIMEOUT 4
SSOLED ssoled[2];
void setup() {
  int inutil;
    inutil = oledInit(&ssoled[0], MY_OLED1, OLED_ADDR, FLIP180, INVERT, 1, SDA_PIN, SCL_PIN, RESET_PIN, 400000L);
    inutil = oledInit(&ssoled[1], MY_OLED2, OLED_ADDR, FLIP180, INVERT, 0, GROVE_SDA_PIN, GROVE_SCL_PIN, RESET_PIN, 400000L);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(A0, INPUT);
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
  digitalWrite(7, HIGH);
  delay(25);
  digitalWrite(7, LOW);
  Serial.begin(9600);
}

void loop() {
if (digitalRead(2)!= HIGH){
  Serial.println("player left");
} if (digitalRead(3)!= HIGH){
  Serial.println("player right");
} if (digitalRead(4)!= HIGH){
Serial.println("+");
} if (digitalRead(5)!=HIGH){
  Serial.println("pause");
} if (digitalRead(6)!=HIGH){
  Serial.println("-");
}
        oledWriteString(&ssoled[0], 0, 30, 5, (char *)"left", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled[1], 0, 30, 5, (char *)"right", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled[0], 0, 0, 3, (char *)"TEST", FONT_STRETCHED, 0, 1);
        oledWriteString(&ssoled[1], 0, 0, 3, (char *)"TEST", FONT_STRETCHED, 0, 1);
}
