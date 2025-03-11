// ######################
// #   DME CHESS CLOCK  #
// #     FOR ARDUINO    #
// ######################
// https://github.com/Jordintown/ChessClock


#include "chessclock-kernel.h"

struct volatil {
  long t0;
  long t1;
  int bonus;
};

clase_setup *eeprom;  //  puntero a la configuracion
clase_hardware *hardware;  //  puntero al hadrware
clase_tiempo *tiempo;      //  puntero al reloj
volatil config;


void setup(){
  Serial.begin(115200);
  hardware=new clase_hardware();     // instanciamos el control de hardware en memoria dinamica
  eeprom=new clase_setup();  // instanciamos el acceso a EEPROM en memoria dinamica
  if(hardware->botonPulsado()==(PULSADO_MAS|PULSADO_MENOS)){ // inicializacion de la EEPROM al estado de fabrica
    eeprom->setTiempo(600000);
    eeprom->setBonus(0);
    eeprom->commit();
  }
  tiempo=new clase_tiempo(hardware,eeprom->getTiempo(),eeprom->getBonus()); // instanciamos el RTC en memoria dinamica
  config.t0=tiempo->consultaTiempo(0);
  config.t1=tiempo->consultaTiempo(1);
  config.bonus=tiempo->consultaBonus();
  hardware->refrescaPantallas(config.t0,config.t1);
  hardware->beep(10);
}



//  **********



void set(void){
  hardware->borra(0);
  hardware->imprime(0, 2, 2, FONT_LARGE, 0, String("setup"));
  hardware->imprimetiempo(1, config.bonus);
  while(!hardware->botonPulsado());
  tiempo->set(config.t0,config.t1,config.bonus);
}



//  **********



void loop(){

// interaccion con los botones

unsigned short pulsado=hardware->botonPulsado();

switch(tiempo->activo()){

  // si ha acabado el tiempo solo sabemos entrar en setup, cuando lo pidamos

  case 3:

    if(pulsado==(PULSADO_MAS|PULSADO_MENOS)){
      while(hardware->botonPulsado()); // se mantiene en el bucle mientras haya algun boton pulsado
      set();
      hardware->refrescaPantallas(tiempo->consultaTiempo(0),tiempo->consultaTiempo(1));
    }
  
  break;

  // pero si estamos cronometrando...

  case 0:
  case 1:
  case 2:

  // ...reaccionamos a los botones

  switch(pulsado){

    case PULSADO_P1:    // el jugador 1 ha pulsdo su boton
      if(tiempo->activo()!=0){    // este if evita incrementos incontrolados
        tiempo->cambiaJugador(0);
        hardware->imprime(1,0,0,FONT_SMALL,0,String("<<<<<"));
      }
    break;
    
    case PULSADO_P2:    // el jugador 0 ha pulsado su boton
      if(tiempo->activo()!=1){   // este if evita incrementos incontrolados
        tiempo->cambiaJugador(1);  
        hardware->imprime(1,0,0,FONT_SMALL,0,String(">>>>>"));
      }
    break;

    case PULSADO_PAUSA: // pausa solicitada
      tiempo->cambiaJugador(2);
      hardware->imprime(1,0,0,FONT_SMALL,0,String("pause"));
      while(hardware->botonPulsado()); // se mantiene en el bucle mientras haya algun boton pulsado
    break;
    
    case (PULSADO_MAS|PULSADO_MENOS):
      while(hardware->botonPulsado()); // se mantiene en el bucle mientras haya algun boton pulsado
      if(tiempo->activo()==2){         // solo entraremos en setup si estamos en pausa
        set();
        hardware->refrescaPantallas(tiempo->consultaTiempo(0),tiempo->consultaTiempo(1));
      }
    break;

  }

  // ...y cambiamos de estado si hemos sobrepasado el cero

  if((tiempo->consultaTiempo(tiempo->activo()))<0){
    hardware->imprimetiempo(tiempo->activo(), -1);  // visualizacion de fin de juego
    hardware->imprime(1,0,0,FONT_SMALL,0,String("GAME OVER"));
    hardware->gameover(tiempo->activo());
    tiempo->cambiaJugador(3);                       // estado de fin de juego
    hardware->beep(1000);                           // zumbido de fin de juego
  }

  break;

}


// latido del corazón, que da la oportunidad a que el RTC haga sus cosas
tiempo->heartbeat();
}

