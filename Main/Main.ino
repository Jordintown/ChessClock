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

SSOLED ssoled[2];

unsigned long clock, segundos, ultimoclock;
unsigned long segundos1 = 600;  // 10 minutos en segundos
unsigned long segundos2 = 600;  // 10 minutos en segundos
const unsigned long bonus = 00; // Bonus de 10 segundos por movimiento
int moves1 = 0, moves2 = 0;     // Contadores de movimientos
byte balancinstat, balancinstat2;
int player = 0;
bool boton1Presionado = false;
bool boton2Presionado = false;
bool pause = true;

void setup() {
  int rc;
  rc = oledInit(&ssoled[0], MY_OLED1, OLED_ADDR, FLIP180, INVERT, 1, SDA_PIN, SCL_PIN, RESET_PIN, 400000L);
  rc = oledInit(&ssoled[1], MY_OLED2, OLED_ADDR, FLIP180, INVERT, 0, GROVE_SDA_PIN, GROVE_SCL_PIN, RESET_PIN, 400000L);
  
  oledFill(&ssoled[0], 0, 1);
  oledWriteString(&ssoled[0], 0, 0, 0, (char *)"DME Chess Clock", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[0], 0, 10, 3, (char *)"INIT", FONT_STRETCHED, 0, 1); // Mensaje inicial para Jugador 1
  
  oledFill(&ssoled[1], 0, 1);
  oledWriteString(&ssoled[0], 0, 0, 0, (char *)"vINT X", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[0], 0, 10, 3, (char *)"INIT", FONT_STRETCHED, 0, 1); // Mensaje inicial para Jugador 1
  
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
  mostrarTextoSuperior(&ssoled[1], segundos2);  // Mostrar hr/min o min/sec
  mostrarTiempoRestante(&ssoled[1], segundos2);  // Mostrar tiempo restante
  mostrarBonus(&ssoled[1]);  // Mostrar "Bonus" en la pantalla del Jugador 2
  mostrarMovimientos(&ssoled[1], moves2);  // Mostrar movimientos del Jugador 2 (inicialmente 0)
  oledWriteString(&ssoled[1], 0, 0, 0, (char *)"Moves: 0", FONT_NORMAL, 0, 1);  // Contador de movimientos
}

// Función para convertir segundos a hh:mm o mm:ss
void mostrarTiempoRestante(SSOLED *oled, unsigned long segundosRestantes) {
  char buffer[6];
  if (segundosRestantes >= 3600) {  // Si hay más de 1 hora
    int horas = segundosRestantes / 3600;
    int minutos = (segundosRestantes % 3600) / 60;
    sprintf(buffer, "%02d:%02d", horas, minutos);  // Mostrar solo horas y minutos
  } else {  // Menos de 1 hora
    int minutos = segundosRestantes / 60;
    int segundos = segundosRestantes % 60;
    sprintf(buffer, "%02d:%02d", minutos, segundos);  // Mostrar minutos y segundos
  }
  oledWriteString(oled, 0, 20, 3, buffer, FONT_STRETCHED, 0, 1);
}

// Función para actualizar el texto en la parte superior
void mostrarTextoSuperior(SSOLED *oled, unsigned long segundosRestantes) {
  if (segundosRestantes >= 3600) {
    oledWriteString(oled, 0, 85, 0, (char *)"hr/min", FONT_SMALL, 0, 1);  // Cambia a hr/min
  } else {
    oledWriteString(oled, 0, 85, 0, (char *)"min/sec", FONT_SMALL, 0, 1);  // Cambia a min/sec
  }
}

// Función para mostrar "Bonus" en la parte inferior izquierda de la pantalla
void mostrarBonus(SSOLED *oled) {
  //oledWriteString(oled, 0, 0, 7, (char *)"Bonus", FONT_SMALL, 0, 1);  // Mostrar "Bonus" en la esquina inferior izquierda
}

// Función para mostrar movimientos
void mostrarMovimientos(SSOLED *oled, int movimientos) {
  char buffer[10];
  sprintf(buffer, "Moves: %d", movimientos);
  oledWriteString(oled, 0, 0, 0, buffer, FONT_NORMAL, 0, 1);
}

void loop() {
  clock = millis();
  segundos = clock / 1000;
  /*if (pause == true){
    oledWriteString(&ssoled[0], 0, 85, 7, (char *)"Pause", FONT_NORMAL, 0, 1);
  }
  else{
    oledWriteString(&ssoled[0], 0, 85, 7, (char *)"Play", FONT_NORMAL, 0, 1);
  }*/
  if (clock > (ultimoclock + 999)) {
    ultimoclock = clock;

    if (player == 0) {  // Jugador 1
      if (segundos1 > 0) {
        if (pause == false){
          segundos1--;
        }
      }
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);

      mostrarTextoSuperior(&ssoled[0], segundos1);  // Mostrar hr/min o min/sec
      mostrarTiempoRestante(&ssoled[0], segundos1);  // Mostrar tiempo restante
      mostrarBonus(&ssoled[0]);  // Mostrar "Bonus" en la pantalla del Jugador 1
      mostrarMovimientos(&ssoled[0], moves1);  // Mostrar movimientos del Jugador 1

      if (segundos1 == 0) {
        digitalWrite(9, HIGH);
        delay(500);
      }

    } else {  // Jugador 2
      if (segundos2 > 0) {
        if (pause == false){
          segundos2--;
        }
      }
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);

      mostrarTextoSuperior(&ssoled[1], segundos2);  // Mostrar hr/min o min/sec
      mostrarTiempoRestante(&ssoled[1], segundos2);  // Mostrar tiempo restante
      mostrarBonus(&ssoled[1]);  // Mostrar "Bonus" en la pantalla del Jugador 2
      mostrarMovimientos(&ssoled[1], moves2);  // Mostrar movimientos del Jugador 2

      if (segundos2 == 0) {
        digitalWrite(10, HIGH);
        delay(500);
      }
    }
  }

  // Detectar el botón del Jugador 1
  if (digitalRead(2) == LOW && !boton1Presionado && player == 0) {  
    player = 1;  // Cambia al Jugador 2
    segundos2 += bonus;  // Agregar bonus al Jugador 2
    moves1++;  // Incrementar movimientos del Jugador 1
    boton1Presionado = true;
    Serial.println("Cambio a Jugador 2");
  }
  
  if (digitalRead(2) == HIGH) {
    boton1Presionado = false;
  }

  // Detectar el botón del Jugador 2
  if (digitalRead(3) == LOW && !boton2Presionado && player == 1) {  
    player = 0;  // Cambia al Jugador 1
    segundos1 += bonus;  // Agregar bonus al Jugador 1
    moves2++;  // Incrementar movimientos del Jugador 2
    boton2Presionado = true;
    Serial.println("Cambio a Jugador 1");
  }
  
  if (digitalRead(3) == HIGH) {
    boton2Presionado = false;
  }

  if (digitalRead(5) == HIGH) {
    if (pause == true){
      pause = false;
      //oledWriteString(&ssoled[0], 0, 85, 7, (char *)"Play", FONT_NORMAL, 0, 1);
    }
    else
    {
      pause = true;
      //oledWriteString(&ssoled[0], 0, 85, 7, (char *)"Pause", FONT_NORMAL, 0, 1);
    }
  }
}
