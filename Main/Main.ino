// ######################
// #   DME CHESS CLOCK  #
// #     FOR ARDUINO    #
// ######################
// https://github.com/Jordintown/ChessClock

#include <ss_oled.h>
#include <EEPROM.h>
#include <RTC.h>


// desde aquí la funcion que detecta la pulsacion de losbotones y la definicion de las mascaras de bits.
//  Por el momento no es Orientada a Objetos

#define PULSADO_P1    0b00000001
#define PULSADO_P2    0b00000010
#define PULSADO_MAS   0b00000100
#define PULSADO_PAUSA 0b00001000
#define PULSADO_MENOS 0b00010000

#define SZ_MUESTREO_BAT 20
#define MUESTREO_MILLIS 200

class clase_pantalla{
  private:
  SSOLED ssoled;
  uint8_t DUbuffer[1024];

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
    int seg=tiempo%60;
    int min=(tiempo/60)%60;
    int hr=min/60;
    sprintf(buffer, "%2d:%02d:%02d", hr, min, seg);
    String texto=String(buffer);
    imprime(0,3,FONT_LARGE,0,texto);
  }

};

class clase_hardware{
private:
  clase_pantalla *pant[2];
  int bat[SZ_MUESTREO_BAT];
  short muestraBat;
  unsigned long stopBeep;
  unsigned long tocaMuestrear;

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
    pant[1]=new clase_pantalla(A4,A5);
    pant[0]=new clase_pantalla(A2,A3);
    for(muestraBat=0;muestraBat<SZ_MUESTREO_BAT;muestraBat++){
      bat[muestraBat]=analogRead(0);
    }
  }

  void periodico(unsigned long t0,unsigned long t1){
    if(millis()>stopBeep){
      digitalWrite(7,LOW);
    }
    if(millis()>tocaMuestrear){
      tocaMuestrear=millis()+MUESTREO_MILLIS;
      muestreoBat();
    }
    pant[0]->imprimetiempo(t0);
    pant[1]->imprimetiempo(t1);
  }

  void imprime(int p, int x, int y, int tam, int inv, String s){
    if((p==0)||(p==1)){
      pant[p]->imprime(x, y, tam, inv, s);
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

class clase_tiempo{
  unsigned int jugadorActivo;
  unsigned long ultimoAcceso;
  unsigned long tiempoJugador[2];
  unsigned long millisJugador[2];
  clase_hardware *hard;

private:

  unsigned long tiempoaescribir(unsigned int jugador){
    unsigned long devolver=0;
    if((jugador==0)||(jugador==1)){
      if(jugador==jugadorActivo){
        devolver=(millisJugador[jugador]-millis())/1000;
Serial.println(devolver);
      }else{
        devolver=tiempoJugador[jugador]/1000;
      }
    }else{
      devolver=tiempoJugador[jugador]/1000;
    }
    return(devolver);
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
  
  void heartbeat(){
    hard->periodico(tiempoaescribir(0),tiempoaescribir(1));
//    if((tiempoaescribir(activo())==0)&&((jugadorActivo==0)||(jugadorActivo==1))){
//      jugadorActivo=2;
//      hard->beep(1000);
//    }
  }

  void cambiaJugador(int j){
    if((j==0)||(j==1)){
      tiempoJugador[jugadorActivo]=millisJugador[jugadorActivo]-millis();
      millisJugador[jugadorActivo]=0;
      millisJugador[j]=tiempoJugador[j]+millis();
    }
    jugadorActivo=j;
  }

  unsigned int activo(){
    return jugadorActivo;
  }
};


clase_hardware *hardware;
clase_tiempo *tiempo;

void setup(){
  Serial.begin(115200);
  hardware=new clase_hardware();
  tiempo=new clase_tiempo(hardware);
  hardware->beep(25);
}

void loop(){
//  String texto;
tiempo->heartbeat();
//Serial.println(tiempo->activo());

unsigned short pulsado=hardware->botonPulsado();
switch(pulsado){

  case PULSADO_P1:
    tiempo->cambiaJugador(0);
  break;
  
  case PULSADO_P2:
    tiempo->cambiaJugador(1);
  break;

  case PULSADO_PAUSA:
    tiempo->cambiaJugador(2);
  break;
  
  case PULSADO_MAS:
    hardware->beep(100);
  break;

  case PULSADO_MENOS:
    hardware->beep(800);
  break;

}


}

