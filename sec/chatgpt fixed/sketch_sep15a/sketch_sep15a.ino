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

#define PAUSE 3

SSOLED ssoled[2];

unsigned long clock, segundos, ultimoclock;
unsigned long segundosJugador[2];
int bonus;
int moves[2];     // Contadores de movimientos
int player = PAUSE;
bool botonPresionado[2];

void setup() {
  int inutil;
  inutil = oledInit(&ssoled[0], MY_OLED1, OLED_ADDR, FLIP180, INVERT, 1, SDA_PIN, SCL_PIN, RESET_PIN, 400000L);
  inutil = oledInit(&ssoled[1], MY_OLED2, OLED_ADDR, FLIP180, INVERT, 0, GROVE_SDA_PIN, GROVE_SCL_PIN, RESET_PIN, 400000L);
  
  for(int i=0;i<2;i++){
    segundosJugador[i]=300;  // 10 minutos en segundos
    bonus=3;
    moves[i]=0;
    botonPresionado[i]=0;
    oledFill(&ssoled[i], 0, 1);
    //oledWriteString(&ssoled[0], 0, 0, 0, (char *)"DME Chess Clock", FONT_NORMAL, 0, 1);
    //oledWriteString(&ssoled[1], 0, 0, 0, (char *)"vINT X", FONT_NORMAL, 0, 1);
    //oledWriteString(&ssoled[i], 0, 10, 3, (char *)"INIT", FONT_STRETCHED, 0, 1); // Mensaje inicial
  }  
  Serial.begin(9600);
  ultimoclock = 0;

  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
  timeSetting(0);
}

void timeSetting(int type){
  if (type == 0) { // initial sestting when u start the clock
    while (digitalRead(5)==HIGH){
    oledWriteString(&ssoled[0], 0, 0, 0, (char *)"Time setting", FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 7, (char *)"PAUSE: confirm", FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[0], 0, 10, 1, (char *)"Time:", FONT_SMALL, 0, 1);
    oledWriteString(&ssoled[1], 0, 0, 7, (char *)"+/-: modify)", FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[0], 0, 20, 3, (char *)"XX:XX", FONT_STRETCHED, 0, 1);
    }
  } else if (type ==1){

  }
   else Serial.println("ERROR: parametro incorrecto");
}
// Función para convertir segundos a hh:mm o mm:ss
void mostrarTiempoRestante(SSOLED *oled, unsigned long segundosRestantes) {
  char buffer[6];
  if (segundosRestantes >= 3600) {  // Si hay más de 1 hora
    int horas = segundosRestantes / 3600;
    int minutos = (segundosRestantes % 3600) / 60;
    int segundos = segundosRestantes % 60;
    sprintf(buffer, "%2d:%02d:%02d", horas, minutos, segundos);  // Mostrar solo horas y minutos
    oledWriteString(oled, 0, -1, 3, buffer, FONT_STRETCHED, 0, 1);
  } else {  // Menos de 1 hora
    int minutos = segundosRestantes / 60;
    int segundos = segundosRestantes % 60;
    sprintf(buffer, "%02d:%02d      ", minutos, segundos);  // Mostrar minutos y segundos
    oledWriteString(oled, 0, 18, 3, buffer, FONT_STRETCHED, 0, 1);
  }
  
}

// Función para actualizar el texto en la parte superior
/*void mostrarTextoSuperior(SSOLED *oled, unsigned long segundosRestantes) {
  if (segundosRestantes >= 3600) {
    oledWriteString(oled, 0, 85, 0, (char *)"hr/min", FONT_SMALL, 0, 1);  // Cambia a hr/min
  } else {
    oledWriteString(oled, 0, 85, 0, (char *)"min/sec", FONT_SMALL, 0, 1);  // Cambia a min/sec
  }
}*/

// Función para mostrar "Bonus" en la parte inferior izquierda de la pantalla
void mostrarBonus() {
  if (bonus!=0){
      oledWriteString(&ssoled[1], 0, 0, 7, "bonus", FONT_SMALL, 0, 1);
  }
}

// Función para mostrar movimientos
void mostrarMovimientos(SSOLED *oled, int movimientos) {
  char buffer[10];
  sprintf(buffer, "Moves: %d", movimientos);
  oledWriteString(oled, 0, 0, 0, buffer, FONT_NORMAL, 0, 1);
}

void refrescaDisplay(SSOLED *pantalla, unsigned long segunds, int mov)
{
//	mostrarTextoSuperior(pantalla, segunds);  // Mostrar hr/min o min/sec
  mostrarTiempoRestante(pantalla, segunds);  // Mostrar tiempo restante
  mostrarBonus();  // Mostrar "Bonus" en la pantalla del Jugador 1
  mostrarMovimientos(pantalla, mov);  // Mostrar movimientos del Jugador 1

}
/*void Mensaje(char Texto){
  oledWriteString(oled, 0, 0, 7, (char *)Texto, FONT_SMALL, 0, 1);  // Mostrar "Bonus" en la esquina inferior izquierda
}*/

void loop() {
  clock = millis();
  segundos = clock / 1000;

  if (clock > (ultimoclock + 999)) {
    ultimoclock = clock;

    switch(player)
    {
    case 0:
    oledWriteString(&ssoled[0], 0, 85, 7, (char *)"Play ", FONT_NORMAL, 0, 1);
    if (segundosJugador[0] > 0) {
          segundosJugador[0]--;
      }
//      digitalWrite(12, HIGH);
//      digitalWrite(11, LOW);
      if (segundosJugador[0] == 0) {
        digitalWrite(9, HIGH);
        delay(500);
      }
    break;

    case 1:
    oledWriteString(&ssoled[0], 0, 85, 7, (char *)"Play ", FONT_NORMAL, 0, 1);
      if (segundosJugador[1] > 0) {
          segundosJugador[1]--;
      }
//      digitalWrite(11, HIGH);
//      digitalWrite(12, LOW);
      if (segundosJugador[1] == 0) {
        digitalWrite(10, HIGH);
        delay(500);
      }
    break;

    case PAUSE:
      oledWriteString(&ssoled[0], 0, 85, 7, (char *)"Pause", FONT_NORMAL, 0, 1);
    break;

    default:
      oledWriteString(&ssoled[0], 0, 85, 7, (char *)"ERROR", FONT_NORMAL, 0, 1);
      Serial.println("Error: estado desconocido");
    }

    for(int i=0;i<2;i++){
     refrescaDisplay(&ssoled[i],segundosJugador[i], moves[i]);
    }

  }

  // Detectar el botón del Jugador 1
  if (digitalRead(2) == LOW && !botonPresionado[0] && (player == 0 || player==PAUSE)) {  
    player = 1;  // Cambia al Jugador 2
    segundosJugador[0] += bonus;  // Agregar bonus al Jugador 2
    moves[0]++;  // Incrementar movimientos del Jugador 1
    botonPresionado[0] = true;
    Serial.println("Cambio a Jugador 2");
  }
  else botonPresionado[0] = false;
  
  if (digitalRead(2) == HIGH) {
    botonPresionado[0] = false;
  }

  // Detectar el botón del Jugador 2
  if (digitalRead(3) == LOW && !botonPresionado[1] && (player == 1 || player==PAUSE)) {  
    player = 0;  // Cambia al Jugador 1
    segundosJugador[1] += bonus;  // Agregar bonus al Jugador 1
    moves[1]++;  // Incrementar movimientos del Jugador 2
    botonPresionado[1] = true;
    Serial.println("Cambio a Jugador 1");
  }
  else botonPresionado[1] = false;
  
  if (digitalRead(3) == HIGH) {
    botonPresionado[1] = false;
  }

// Detectar el botón del PAUSE
  
 if (digitalRead(5) == LOW) {
  player=PAUSE;
  Serial.println("PAUSE");
 }

}
