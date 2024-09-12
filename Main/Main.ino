#include <SPI.h>
#include <Wire.h>
#include <ss_oled.h>
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C OLED1(U8G2_R0,A5,A4,U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C OLED2(U8G2_R0,A3,A2,U8X8_PIN_NONE);

#define GROVE_SDA_PIN A2
#define GROVE_SCL_PIN A3
// These are the pin numbers for the M5Stack Atom default I2C
#define SDA_PIN A4
#define SCL_PIN A5
// Set this to -1 to disable or the GPIO pin number connected to the reset
// line of your display if it requires an external reset
#define RESET_PIN -1
// let ss_oled figure out the display address
#define OLED_ADDR -1
// don't rotate the display
#define FLIP180 0
// don't invert the display
#define INVERT 0
// Bit-Bang the I2C bus
#define USE_HW_I2C 0

// Change these if you're using different OLED displays
#define MY_OLED1 OLED_128x64
#define MY_OLED2 OLED_128x64

// 2 copies of the SSOLED structure. Each structure is about 56 bytes
// There is no limit to the number of simultaneous displays which can be controlled by ss_oled 
SSOLED ssoled[2];

unsigned long clock,segundos,ultimoclock,segundos1,segundos2;
int jugador1=0;
int moves1=0;
int moves2=0;
int longitud;
byte balancinstat;
byte balancinstat2;
int player;
int increment=10;


void setup(){
  //set (temporary) the time control here
  segundos1=1000;
  segundos2=1000;
  //---
  Serial.begin(9600);
  ultimoclock=0;
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop(){

  clock = millis();
  segundos = clock/1000;

  //AIP'd
  /*longitud=Serial.available();
  if (longitud>0) {
      String texte="";
      for(;longitud>0;longitud--){
        texte+=(char)Serial.read();
      }
      Serial.println(texte);
      balancin();
  }*/

  if(clock>(ultimoclock+999)){
    ultimoclock=clock;
    if (player=0){
      segundos1--;
      //Serial.println(String(segundos1)+" | "+String(moves1)+" < "+String(moves2)+" | "+String(segundos2));
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW); 
      //flag fall handling 
        if (segundos1<1);{
        digitalWrite(9, HIGH);
        digitalWrite(11, LOW);
        delay(2000);
    }
    }
    else{
      segundos2--;
      //Serial.println(String(segundos1)+" | "+String(moves1)+" > "+String(moves2)+" | "+String(segundos2));
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);    
      //flag fall handling
      if (segundos2<1);{
        while (segundos2<1){
          digitalWrite(10, HIGH);
          digitalWrite(12, LOW);
          delay(2000);
        }
    }
  }

  balancinstat = digitalRead(2);
  if (balancinstat == LOW) {
    player=0;
        while(balancinstat == LOW){
        balancinstat = digitalRead(2);
      }
    moves1++;
  }
   balancinstat2 = digitalRead(3);
  if (balancinstat2 == LOW) {
      player=1;
        while(balancinstat2 == LOW){    
        balancinstat = digitalRead(3);  
      }
    moves2++;
    }
  }
}
