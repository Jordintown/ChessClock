// ######################
// #   DME CHESS CLOCK  #
// #     FOR ARDUINO    #
// #     PART OF THE    #
// #  ARDUCHESS PROJECT #
// ######################
// https://github.com/Jordintown/ChessClock

#include <ss_oled.h>
#include <EEPROM.h>
#include <RTC.h>

uint8_t DUbuffer[1024];

class clase_pantalla{
  SSOLED ssoled;

  private:

  public:
  clase_pantalla(int sda, int scl){
    oledSetTextWrap(&ssoled, 1);
    oledInit(&ssoled, OLED_128x64, -1, 0, 0, 0, sda, scl, -1, 400000L);
    oledFill(&ssoled, 0, 1);
    oledSetBackBuffer(&ssoled, DUbuffer);
  }

  void imprime(int x, int y, int tam, int inv, String s){
    int tambuf=s.length()+1;
    char buffer[tambuf];
    s.toCharArray(buffer, tambuf);
    oledWriteString(&ssoled, 0, x, y, buffer, tam, inv, 1);
  }

  void imprimetiempo(unsigned long tiempo){
    char buffer[10];
  /*  unsigned int horas=tiempo/3600;
    unsigned int minutos=(tiempo%3600)/60;
    unsigned int segundos=minutos%60;
    String texto=String(horas);
    texto=texto+":";
    texto=texto+minutos;
    texto=texto+":";
    texto=texto+segundos;*/

    int seg=tiempo%60;
    int min=(tiempo/60)%60;
    int hr=min/60;
    sprintf(buffer, "%2d:%02d:%02d", hr, min, seg);
    String texto=String(buffer);
    imprime(0,3,FONT_LARGE,0,texto);
  }

};

clase_pantalla *pantalla[2];

class clase_tiempo{
  short jugadorActivo;
  unsigned long tiempoJugador[2];
  unsigned long millisJugador[2];
  
  public:
  
  clase_tiempo(){
    for (int i=0;i<2;i++){
      tiempoJugador[i]=600000;
    }
    jugadorActivo=0;
    millisJugador[0]=tiempoJugador[0]+millis();
    millisJugador[1]=0;
  }
  
  void cambiaJugador(int j){
    tiempoJugador[jugadorActivo]=millisJugador[jugadorActivo]-millis();
    millisJugador[jugadorActivo]=0;
    millisJugador[j]=tiempoJugador[j]+millis();
    jugadorActivo=j;
  }
  
  unsigned long tiempoaescribir(){
    return ((millisJugador[jugadorActivo]-millis())/1000);
  }

  short activo(){
    return jugadorActivo;
  }

};

clase_tiempo *tiempo;

void setup(){
  Serial.begin(115200);
  pantalla[0]=new clase_pantalla(A4,A5);
  pantalla[1]=new clase_pantalla(A2,A3);
  tiempo=new clase_tiempo();
}

void loop(){
//  clase_pantalla p2=clase_pantalla(A4,A5);
  RTCTime currentTime;
  RTC.getTime(currentTime);
  /*Serial.print(currentTime.getHour());
  Serial.print(":");
  Serial.print(currentTime.getMinutes());
  Serial.print(":");
  Serial.println(currentTime.getSeconds());
*/
  String texto=String(currentTime);
  pantalla[1]->imprime(0,7,FONT_SMALL,0,texto);
  pantalla[tiempo->activo()]->imprimetiempo(tiempo->tiempoaescribir());
}

