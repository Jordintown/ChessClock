#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,A5,A4,U8X8_PIN_NONE);

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
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5,10,"INIT");
  u8g2.sendBuffer();
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
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(5,10,"Player 1:"+String(segundos1));
      u8g2.drawStr(5,20,"Moves:"+String(moves1));
      u8g2.drawStr(5,30,"<");
      u8g2.drawStr(5,40,"Player 2:"+String(segundos2));
      u8g2.drawStr(5,50,"Moves:"+String(moves2));
      u8g2.sendBuffer();
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
      segundos1--;
      //Serial.println(String(segundos1)+" | "+String(moves1)+" > "+String(moves2)+" | "+String(segundos2));
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(5,10,"Player 1:"+String(segundos1));
      u8g2.drawStr(5,20,"Moves:"+String(moves1));
      u8g2.drawStr(5,30,">");
      u8g2.drawStr(5,40,"Player 2:"+String(segundos2));
      u8g2.drawStr(5,50,"Moves:"+String(moves2));
      u8g2.sendBuffer();
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
/*        if(player==0){
      moves1++;
      segundos1--;
      Serial.println(String(segundos1)+" | "+String(moves1)+" < "+String(moves2)+" | "+String(segundos2));
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
      moves2++;
      Serial.println(String(segundos1)+" | "+String(moves1)+" > "+String(moves2)+" | "+String(segundos2));
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
    /*if(balancinstat == HIGH){
      while(balancinstat == HIGH){
        delay(1000);
      }*/
  }
}

/*void balancin(int player){
      if(player==0){
        while
      moves1++;
      segundos1--;
      Serial.println(String(segundos1)+" | "+String(moves1)+" < "+String(moves2)+" | "+String(segundos2));
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
      moves2++;
      Serial.println(String(segundos1)+" | "+String(moves1)+" > "+String(moves2)+" | "+String(segundos2));
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
    if(balancinstat == HIGH){
      while(balancinstat == HIGH){
        delay(1000);
      }
    }
}*/  