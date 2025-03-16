// ######################
// #   DME CHESS CLOCK  #
// #     FOR ARDUINO    #
// ######################
// https://github.com/Jordintown/ChessClock


#include "chessclock-kernel.h"

clase_setup *eeprom;  //  puntero a la configuracion
clase_hardware *hardware;  //  puntero al hadrware
clase_tiempo *tiempo;      //  puntero al reloj


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
  hardware->refrescaPantallas(tiempo->consultaTiempo(0),tiempo->consultaTiempo(1));
  hardware->beep(10);
}

struct volatil {
  long t0;
  long t1;
  long bonus;
};



//  **********  inicio seccion setup



void muestraMenu(int cursor,char *texto[],long *menu[],int menutam){
    if(cursor>0){
      hardware->imprime(0, 10, 1, FONT_SMALL, 0, String(texto[cursor-1]));
    }else{
      hardware->imprime(0, 10, 1, FONT_SMALL, 0, String("        "));
    }
    if(cursor<menutam){
      hardware->imprime(0, 10, 7, FONT_SMALL, 0, String(texto[cursor+1]));
    }else{
      hardware->imprime(0, 10, 7, FONT_SMALL, 0, String("        "));
    }
    hardware->imprime(0, 0, 3, FONT_LARGE, 0, String(texto[cursor]));
    hardware->imprimetiempo(1, *menu[cursor]);
}

int navegaMenu(char *texto[],long *menu[],int menutam){
  int pantallamenu;
  int pantallatiempo;
  int boton;
  unsigned long pulsando;
  int cursor=0;
  muestraMenu(cursor,texto,menu,menutam);
  do{
    boton=hardware->botonPulsado();
    switch(boton){
      case PULSADO_P1:
        if(cursor<menutam){
          cursor++;
        }
        muestraMenu(cursor,texto,menu,menutam);
        while(hardware->botonPulsado());
      break;
      case PULSADO_P2:
        if(cursor>0){
          cursor--;
        }
        muestraMenu(cursor,texto,menu,menutam);
        while(hardware->botonPulsado());
      break;
      case PULSADO_MAS:
        pulsando=millis()+1000;
        *menu[cursor]=*menu[cursor]+1000;
        hardware->imprimetiempo(1, *menu[cursor]);
        while(hardware->botonPulsado()){
          if(pulsando<millis()){
            pulsando=millis()+200;
            *menu[cursor]=*menu[cursor]+60000;
            hardware->imprimetiempo(1, *menu[cursor]);
          }
        }
      break;
      case PULSADO_MENOS:
        pulsando=millis()+1000;
        *menu[cursor]=*menu[cursor]-1000;
        if(*menu[cursor]<0) *menu[cursor]=0;
        hardware->imprimetiempo(1, *menu[cursor]);
        while(hardware->botonPulsado()){
          if(pulsando<millis()){
            pulsando=millis()+200;
            *menu[cursor]=*menu[cursor]-60000;
            if(*menu[cursor]<0) *menu[cursor]=0;
            hardware->imprimetiempo(1, *menu[cursor]);
          }
        }
      break;
//      default:
//        muestraMenu(cursor,texto,menu,menutam);
    }
  }while(boton!=PULSADO_PAUSA);
  hardware->borra(0);
  hardware->borra(1);
  hardware->imprime(0, 0, 2, FONT_LARGE, 0, String(" GUARDA"));
  hardware->imprime(1, 0, 2, FONT_LARGE, 0, String("DESCARTA"));
  do{
    boton=hardware->botonPulsado();
  }while(!(boton&(PULSADO_P1|PULSADO_P2)));
  if(boton&PULSADO_P2)
    return 1;
  else
    return 0;
}

void set(void){
  volatil estado;
  persistente config;
  unsigned short pulsado;
  const int tam_menu=2;  // el numero de elementos del menu de modificacion de partida
  char *texto[tam_menu+1];
  long *menu[tam_menu+1];
  estado.t0=tiempo->consultaTiempo(0);
  estado.t1=tiempo->consultaTiempo(1);
  estado.bonus=tiempo->consultaBonus();
  config.tiempo=eeprom->getTiempo();
  config.bonus=eeprom->getBonus();
  hardware->borra(0);
  hardware->borra(1);
  hardware->imprime(0, 0, 2, FONT_LARGE, 0, String("MODIFICA"));
  hardware->imprime(1, 24, 2, FONT_LARGE, 0, String("NUEVA"));
  do{
    pulsado=hardware->botonPulsado();
  }while(!(pulsado&(PULSADO_P1|PULSADO_P2)));
  while(!hardware->botonPulsado());
  if(pulsado==PULSADO_P1){
    const int menutam=1;
    texto[0]="TIEMPO  ";
    menu[0]=&config.tiempo;
    texto[1]="BONUS   ";
    menu[1]=&config.bonus;
    hardware->borra(1);
    hardware->borra(0);
    hardware->imprime(1, 0, 0, FONT_SMALL, 0, String("NUEVA PARTIDA"));
    if(navegaMenu(texto,menu,menutam)){
      config.tiempo=*menu[0];
      config.bonus=*menu[1];
      eeprom->setTiempo(config.tiempo);
      eeprom->setBonus(config.bonus);
      eeprom->commit();
      hardware->beep(1);
      delay(50);
      hardware->beep(0);
      tiempo->set(eeprom->getTiempo(),eeprom->getTiempo(),eeprom->getBonus());
    }
  }else{
    texto[0]="<-TIEMPO";
    menu[0]=&estado.t0;
    texto[1]="TIEMPO->";
    menu[1]=&estado.t1;
    texto[2]="BONUS   ";
    menu[2]=&estado.bonus;
    hardware->borra(1);
    hardware->borra(0);
    hardware->imprime(1, 0, 0, FONT_SMALL, 0, String("MODIFICACION"));
    if(navegaMenu(texto,menu,tam_menu)){
      estado.t0=*menu[0];
      estado.t1=*menu[1];
      estado.bonus=*menu[2];
      tiempo->set(estado.t0,estado.t1,estado.bonus);
    }
  }
}



//  **********  fin seccion setup



// si pulsamos un boton durante el juego movemos pieza...

void muevoPieza(unsigned short pulsado){
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
}

void loop(){

// interaccion con los botones
unsigned short pulsado=hardware->botonPulsado();
muevoPieza(pulsado);

// latido del corazón, que da la oportunidad a que el RTC haga sus cosas
tiempo->heartbeat();

}

