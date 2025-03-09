#include <ss_oled.h>


// desde aquí el metodo que detecta la pulsacion de los botones y la definicion de las mascaras de bits.
//  Por el momento no es Orientada a Objetos

#define PULSADO_P2    0b00000001
#define PULSADO_P1    0b00000010
#define PULSADO_MAS   0b00000100
#define PULSADO_PAUSA 0b00001000
#define PULSADO_MENOS 0b00010000


// constantes para el muestro de bateria

#define SZ_MUESTREO_BAT 10    // numero de lecturas de las que se calcula la media
#define MUESTREO_MILLIS 500   // milisegundos entre lecturas del voltaje
#define REFRESCO_MILLIS 2000  // milisegundos entre escrituras en las pantallas


// constantes para el calculo de voltaje de bateria

#define V_REF 5              // tipico    5V: voltaje de referencia del arduino
#define BITS_RESOLUCION 14   // tipico 10bit: resolucion del conversor ADC
#define TENSION_DIODO 1.1    // tipico  1.1V: caida de tension del diodo de proteccion 1N4001
#define DESVIACION 0.4       // Tipico  0.0V: para compensar caidas de tension imprevistas


// constantes para el calculo de porcentaje de bateria

#define BAT_V_NOMINAL 7.4 // voltaje nominal de la bateria
#define BAT_PERCENT 10.   // tipico entre +-10 y +-15%: porcentaje de carga-descarga de la bateria



class clase_pantalla{
  private:
  SSOLED ssoled;
//  uint8_t DUbuffer[1024];

  public:
  clase_pantalla(int sda, int scl){
    oledSetTextWrap(&ssoled, 1);
    oledInit(&ssoled, OLED_128x64, -1, 0, 0, 0, sda, scl, -1, 400000L);
//    oledSetBackBuffer(&ssoled, DUbuffer);
  }

// este metodo sirve para borrar la pantalla

  void borra(){
   oledFill(&ssoled, 0, 1);
  }

// este metodo imprime el grafico de fin de partida

  void gameover(){
    imprime(0,6,FONT_STRETCHED,1,String("GameOver"));
  }

// Metodo general para mostrar cualquier texto en cualquier lugar de la pantalla

  void imprime(int x, int y, int tam, int inv, String s){
    int tambuf=s.length()+1;
    char buffer[tambuf];
    s.toCharArray(buffer, tambuf);
    oledWriteString(&ssoled, 0, x, y, buffer, tam, inv, 1);
  }

// publicamos el metodo que permite imprimir rectangulos

  void rectangulo(int x1, int y1, int x2, int y2, int color, int relleno){
    oledRectangle(&ssoled, x1, y1, x2, y2, color, relleno);
  }

// Metodo especifico para visualizar el tiempo en el centro de la pantalla
//  usarlo siempre mostrará el tiempo siempre de la misma forma, evitando ambiguedades

  void imprimetiempo(long tiempo){
    char buffer[10];
    if(tiempo>0){
      tiempo=tiempo/100; // descarto decimas y centesimas
      int dec=tiempo%10; // calculo las decimas
      tiempo=tiempo/10;
      int seg=tiempo%60; // calculo los segundos
      tiempo=tiempo/60;
      int min=tiempo%60; // calculo los minutos
      tiempo=tiempo/60;  // Ahora en la variable tiempo quedan las horas
      if(tiempo){      // si hay algo que mostrar en el campo hora, dejamos espacio para las horas
        sprintf(buffer, "%02d:%02d:%02d", tiempo, min, seg); // HH:MM:SS
      }else{
        if(min){       // si queda mas de un minuto y menos que una hora, el fomrateamos minuto:segundo
          sprintf(buffer, " %02d:%02d  ", min, seg);         // MM:SS
        }else{         // si queda menos de un minuto, dejamos espacio tambien para las decimas de segundo
          sprintf(buffer, " 00:%02d.%d", seg, dec);   // MM:SS.d
        }
      }
    }else{
      sprintf(buffer, "*00:00.0");  //  si el valor es negativo, muestra un asterisco a la izquierda del cero
    }
    String texto=String(buffer);    // creamos un objeto de la clase String con el texto que hemos formateado
    imprime(0,3,FONT_LARGE,0,texto);     //  por fin, visualizamos el texto que hemos calculado.
  }

};


class clase_hardware{
private:
  clase_pantalla *pant[2];
  int bat[SZ_MUESTREO_BAT];
  short muestraBat;
  unsigned long stopBeep;
  unsigned long tocaMuestrear=0;
  unsigned long tocaRefrescar=0;

// este metodo sustituye el valor mas antiguo de la cola circular
//   por la lectura actual del sensor de bateria.
//   lo llamara el metodo periodica a su debido tiempo.

  void muestreoBat(){
    if(muestraBat>=SZ_MUESTREO_BAT){
      muestraBat=0;
    }
    bat[muestraBat++]=analogRead(0);
  }

public:

// acota j entre 0 y 1. Se usara cada vez que toque acceder a un vector de jugadores
//  sirve para no salirnos nunca del vector.
//  como esta comparacion estaba en copia-pega en multiples puntos del codigo
//  he decidido crear esta metodo para mejorar la legibilidad del codigo
//  y ahorrar en memoria de programa a costa de un poquito de pila del sistema

  int valida(int j){
    return((j==0)||(j==1));
  }

  clase_hardware(){                   //  constructor de la clase
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
    analogReadResolution(BITS_RESOLUCION);  //  bits de la lectura del sensor de bateria
    digitalWrite(7,LOW);       //  para el zumbador
    for(muestraBat=0;muestraBat<SZ_MUESTREO_BAT;muestraBat++){
      bat[muestraBat]=analogRead(0); //  relleno inicial de la cola circular
    }  //  instanciamos una clase pantalla por cada una de las pantallas
    pant[0]=new clase_pantalla(A4,A5);
    pant[1]=new clase_pantalla(A2,A3);
  }

//  Como hemos instanciado clases en memoria dinamica,
//    es necesario programar un destructor que libere la memoria reservada a las pantallas
//    por si el usuario decidiera liberar la memoria ocupada por una instancia de esta clase

  ~clase_hardware(){
    delete[] pant;
  }

//  este metodo es llamada por el RTC a intervalos irregulares
//    mantiene el tiempo corriendo en la pantalla del jugador activo
//    y coordina los muestreos y refrescos de los valores de bateria
//    tambien es responsable de parar el zumbador cuando sea menester

  void periodico(unsigned int j,long t){
    if(j==0){
      pant[0]->imprimetiempo(t);
    }
    if(j==1){
      pant[1]->imprimetiempo(t);
    }
    if(millis()>stopBeep){
      digitalWrite(7,LOW);
    }
    if(millis()>tocaMuestrear){
      tocaMuestrear=millis()+MUESTREO_MILLIS;
      muestreoBat();
    }
    if(millis()>tocaRefrescar){
      tocaRefrescar=millis()+REFRESCO_MILLIS;
//      imprime(1,0,0,FONT_SMALL,0,String(consultaBatV())+String("V"));
      imprime(1,110,0,FONT_SMALL,0,String(consultaBatPercent())+String("%!"));
    }
  }

//  para que se pueda imprimir cualquier cosa en cualquier pantalla
//    elige la pantalla especificada en el primer parametro
//    y le pasa el resto de valores a su metodo homonimo

  void imprime(int p, int x, int y, int tam, int inv, String s){
    if(valida(p)){
      pant[p]->imprime(x, y, tam, inv, s);
    }
  }

// para imprimir cualquier rectangulo en cualquier posicion

  void rectangulo(int p, int x1, int y1, int x2, int y2, int color, int relleno){
    if(valida(p)){
      pant[p]->rectangulo(x1, y1, x2, y2, color, relleno);
    }
  }

// para refrescar manualmente el tiempo mostrado en una pantalla
//  esto se usa como minimo al cambiar de jugador, para visualizar
//  el incremento aplicado al cambiar el turno

  void imprimetiempo(int p, long t){
    if(valida(p)){
      pant[p]->imprimetiempo(t);
    }
  }

// si no estamos seguros de si la visualizacion de las pantallas es coherente
//  podemos invocar este metodo y pasarle los valores a visualizar
//  por ejemplo justo despues de arrancar o reconfigurar el qeuipo

  void refrescaPantallas(long t0, long t1){
    pant[0]->borra();
    pant[1]->borra();
    imprimetiempo(0,t0);
    imprimetiempo(1,t1);
  }

// Envia la orden de vsualizar el grafico de fin de juego a la pantalla adcuada

  void gameover(int p){
    if(valida(p)){
      pant[p]->gameover();
    }
  }


// inicia el zumbador y define el instante en que dejará de sonar

  void beep(unsigned long duracion){
    stopBeep=millis()+duracion;
    digitalWrite(7,HIGH);
  }

// devuelve el voltaje de la bateria

  float consultaBatV(){
    unsigned long acumulador=0;
    for(int i=0;i<SZ_MUESTREO_BAT;i++){
      acumulador+=bat[i];
    }
    unsigned long medicion=acumulador/SZ_MUESTREO_BAT;
//    imprime(0,0,0,FONT_SMALL,0,String(medicion));
    float voltaje_detectado=(V_REF*medicion)/(pow(2.,BITS_RESOLUCION)-1.); // 2. y 1. indican al compilador que son float
    float voltaje_calculado=(2.*voltaje_detectado)+TENSION_DIODO; // el detector es un diodo en serie con un divisor de tension
    return voltaje_calculado+DESVIACION; // las matematicas son exactas pero las fabricas no, asi que hay que ajustar
  }

// devuelve el porcentaje de carga de la bateria
// rango util de la bateria es desde BAT_V_NOMINAL-BAT_PERCENT% a BAT_V_NOMINAL+BAT_PERCENT%

  int consultaBatPercent(){
    const float delta_volt=BAT_V_NOMINAL*(BAT_PERCENT/50.);
    const float vmin=(BAT_V_NOMINAL-(delta_volt/2.)); // calculamos cuanto porcentaje de bateria es una desvicion de 1V
    int resultat=((consultaBatV() - vmin) / delta_volt)*100.;
    if (resultat < 0) { resultat = 0; }
    if (resultat >= 99) { resultat = 99; }
    return resultat;
  }

// devuelve una mascara de bits que representa el estado de pulsacion de los botones

  unsigned short botonPulsado(){
    unsigned short devolver=0b00000000;
    for(int i=6;i>=2;i--){
      devolver=devolver*=2;    // anyado un cero a la derecha
      if(digitalRead(i)==LOW){
      devolver++;              // pongo a 1 el bit derecho
      }
    }
    return devolver;
  }
};


class clase_tiempo{
  unsigned int jugadorActivo;
  long incremento;
  unsigned long tiempoJugador[2];
  unsigned long millisJugador[2];
  clase_hardware *hard;

  public:
  
// al constructor se le pasa el puntero a un objeto de la clase hardwarel, previamente instanciado
//  esta referencia es muy util porque asi el RTC refresca las pantallas
//  sin tener que controlar explicitamente la visualizacion del tiempo
//  se activan los relojes a 10 minutos sin incremento, solo para asegurarnos de tener valores en rango.

  clase_tiempo(clase_hardware *h){
    hard=h;
    set(600000,600000,0);
  }

// inicializa el reloj a los valores recibidos como parametros

  void set(unsigned long p0,unsigned long p1,long inc){
    tiempoJugador[0]=p0;
    tiempoJugador[1]=p1;
    incremento=inc;
    jugadorActivo=2;
  }

//  esta metodo ha de ser llamada en cada iteracion de loop para que funcione el RTC

  void heartbeat(){
    long tiempo=consultaTiempo(jugadorActivo);
    hard->periodico(jugadorActivo,tiempo);
  }

//  cada cambio de estado se produce llamando a este metodo

  void cambiaJugador(int j){
    if(hard->valida(jugadorActivo)){
      tiempoJugador[jugadorActivo]=millisJugador[jugadorActivo]-millis();
      if(hard->valida(j)){
        tiempoJugador[jugadorActivo]=tiempoJugador[jugadorActivo]+incremento;
        hard->imprimetiempo(jugadorActivo,tiempoJugador[jugadorActivo]);
      }
    }
    if(hard->valida(j)){
      millisJugador[j]=tiempoJugador[j]+millis();
    }
    jugadorActivo=j;
  }

// el reloj es el que controla el estado del reloj.
//   Esta metodo devuelve el jugador activo o estado del reloj.
//   por ejemplo el estado 2 es pausa y el estado 3 es fin de tiempo

  unsigned int activo(){
    return jugadorActivo;
  }

// si queremos saber el tiempo que le queda a un jugador, llamamos a este metodo

  long consultaTiempo(unsigned int jugador){
    long devolver=0;
    if(hard->valida(jugador)){
      if(jugador==jugadorActivo){
        devolver=(millisJugador[jugador]-millis());
      }else{
        devolver=tiempoJugador[jugador];
      }
    }else{
      devolver=0;
    }
    return(devolver);
  }

};
