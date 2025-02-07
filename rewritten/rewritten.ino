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

// desde aquí la funcion que detecta la pulsacion de losbotones y la definicion de las mascaras de bits.
//  Por el momento no es Orientada a Objetos

#define PULSADO_P1    0b00000001
#define PULSADO_P2    0b00000010
#define PULSADO_MAS   0b00000100
#define PULSADO_PAUSA 0b00001000
#define PULSADO_MENOS 0b00010000

#define SZ_MUESTREO_BAT 15
#define MUESTREO_MILLIS 100

class clase_hardware{
  int bat[SZ_MUESTREO_BAT];
  short muestraBat;
  unsigned long stopBeep;
  unsigned long tocaMuestrear;

private:

  void muestreoBat(){
    if(muestraBat>=SZ_MUESTREO_BAT){
      muestraBat=0;
    }
    bat[muestraBat++]=analogRead(0);
  }

public:

  clase_hardware(){
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_TX, OUTPUT);
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
    digitalWrite(7,LOW);
    for(muestraBat=0;muestraBat<SZ_MUESTREO_BAT;muestraBat++){
      bat[muestraBat]=analogRead(0);
    }
  }

  void periodico(){
    if(millis()>stopBeep){
      digitalWrite(7,LOW);
    }
    if(millis()>tocaMuestrear){
      tocaMuestrear=millis()+MUESTREO_MILLIS;
      muestreoBat();
    }
  }

  void beep(unsigned long duracion){
    stopBeep=millis()+duracion;
    digitalWrite(7,HIGH);
  }

  float consultaBatV(){
    unsigned long acumulador=0;
    for(int i=0;i<SZ_MUESTREO_BAT;i++){
      acumulador+=bat[i];
    }
  // Serial.println(((float)(acumulador/TAM_MUESTREO_BAT))/10.0);
    return (((acumulador/SZ_MUESTREO_BAT) + 80.)/102.);
  }

  float consultaBatPercent(){
    float resultat=(consultaBatV() - 6.6) * 62.5;
    if (resultat < 0.) { resultat = 0.; }
    if (resultat >= 100.) { resultat = 100.; }
    return resultat;
    //(volts - 6.6) * 62.5
  }

  unsigned short botonPulsado(){
    unsigned short devolver=0b00000000;
    for(int i=6;i>=2;i--){
      devolver=devolver*=2;    // anyado un cero a la derecha
      if(digitalRead(i)==LOW){
      devolver++;            // pongo a 1 el bit derecho
      }
    }
    return devolver;
  }
};

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

class clase_tiempo{
  short jugadorActivo;
  unsigned long ultimoAcceso;
  unsigned long tiempoJugador[2];
  unsigned long millisJugador[2];
  clase_hardware *hard;

private:

  unsigned long tiempoaescribir(unsigned int jugador){
    if((jugador==0)||(jugador==1)){
      return((millisJugador[jugador]-millis())/1000);
    }else{
      return(-1);
    }
  }

  public:
  
  clase_tiempo(clase_hardware *h){
    for (int i=0;i<2;i++){
      tiempoJugador[i]=900000;
      millisJugador[i]=0;
    }
    jugadorActivo=2;
    hard=h;
  }
  
  void cambiaJugador(int j){
    tiempoJugador[jugadorActivo]=millisJugador[jugadorActivo]-millis();
    millisJugador[jugadorActivo]=0;
    jugadorActivo=j;
    if((j==0)||(j==1)){
      millisJugador[j]=tiempoJugador[j]+millis();
    }
  }

  unsigned long tiempoaescribir(){
    return tiempoaescribir(jugadorActivo);
  }

  short activo(){
    return jugadorActivo;
  }

  void heartbeat(){
    hard->periodico();
  }
};


clase_hardware *hardware;
clase_pantalla *pantalla[2];
clase_tiempo *tiempo;

void setup(){
  Serial.begin(115200);
  pantalla[1]=new clase_pantalla(A4,A5);
  pantalla[0]=new clase_pantalla(A2,A3);
  hardware=new clase_hardware();
  tiempo=new clase_tiempo(hardware);
  hardware->beep(25);
}

void loop(){
  String texto;
  tiempo->heartbeat();

unsigned short pulsado=hardware->botonPulsado();
//Serial.println(pulsado);
switch(pulsado){

  case PULSADO_P1:
    tiempo->cambiaJugador(0);
  break;
  
  case PULSADO_P2:
    tiempo->cambiaJugador(1);
  break;

  case PULSADO_PAUSA:
    texto=String("Pausa");
    pantalla[tiempo->activo()]->imprime(25,3,FONT_STRETCHED,0,texto);
    tiempo->cambiaJugador(2);
  break;
  
  case PULSADO_MAS:
    hardware->beep(500);
  break;

  case PULSADO_MENOS:
    hardware->beep(300);
  break;

}

  if((tiempo->activo()==0)||(tiempo->activo()==1)){
    pantalla[tiempo->activo()]->imprimetiempo(tiempo->tiempoaescribir());
  }

}

