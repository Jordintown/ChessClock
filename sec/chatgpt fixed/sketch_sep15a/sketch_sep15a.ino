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
  
    for(int i = 0; i < 2; i++) {
        segundosJugador[i] = 100;  // 5 minutos en segundos
        bonus = 30;
        moves[i] = 0;
        botonPresionado[i] = 0;
        oledFill(&ssoled[i], 0, 1);
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

void timeSetting(int type) {
    char buf[6];
    if (type == 0) { // Initial setting when starting the clock
      while (digitalRead(5) == HIGH) {
        oledWriteString(&ssoled[0], 0, 0, 0, (char *)"Confirm time settings", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled[0], 0, 0, 1, (char *)"retrieved from memory", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled[0], 0, 0, 7, (char *)"PAUSE: confirm", FONT_NORMAL, 0, 1);
        oledWriteString(&ssoled[0], 0, 0, 6, (char *)"+/-: modify", FONT_NORMAL, 0, 1);
        ToHMS(segundosJugador[0],buf);
        oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
        ToHMS(bonus,buf);
        oledWriteString(&ssoled[1], 0, 0, 5, (char *)buf, FONT_STRETCHED, 0, 1);
      }
      oledFill(&ssoled[0], 0, 1);
      oledFill(&ssoled[1], 0, 1);
    } else if (type == 1) {
      oledFill(&ssoled[0], 0, 1);
      oledFill(&ssoled[1], 0, 1);
      while (1==1){
      oledWriteString(&ssoled[0], 0, 0, 0, (char *)"Clock Settings", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled[0], 0, 0, 2, (char *)"Time", FONT_SMALL, 0, 1);
      if (digitalRead(5)== LOW){
        break;
      }
      }
    } else {
        Serial.println("ERROR: parametro incorrecto");
    }
}

// Función para convertir cualquier variable a string
String variableToString(int value) {
    return String(value);
}

String variableToString(unsigned long value) {
    return String(value);
}

// Para otros tipos, puedes agregar más sobrecargas si es necesario

// Función para convertir segundos a hh:mm o mm:ss
void mostrarTiempoRestante(SSOLED *oled, unsigned long segundosRestantes) {
    char buffer[6];
    if (segundosRestantes >= 3600) {  // Si hay más de 1 hora
        int horas = segundosRestantes / 3600;
        int minutos = (segundosRestantes % 3600) / 60;
        int segundos = segundosRestantes % 60;
        sprintf(buffer, "%2d:%02d:%02d", horas, minutos, segundos);  // Mostrar solo horas y minutos
        oledWriteString(oled, 0, 0, 3, buffer, FONT_STRETCHED, 0, 1);
    } else {  // Menos de 1 hora
        int minutos = segundosRestantes / 60;
        int segundos = segundosRestantes % 60;
        sprintf(buffer, "%02d:%02d   ", minutos, segundos);  // Mostrar minutos y segundos
        oledWriteString(oled, 0, 18, 3, buffer, FONT_STRETCHED, 0, 1);
    }
}

char* ToHMS(unsigned long seg, char *buffer) {
  //char buffer[6];
  if (seg >= 3600) {  // Si hay más de 1 hora
    int horas = seg / 3600;
    int minutos = (seg % 3600) / 60;
    int segundos = seg % 60;
    sprintf(buffer, "%2d:%02d:%02d   ", horas, minutos, segundos);  // Mostrar solo horas y minutos
    return buffer;
  } else {  // Menos de 1 hora
    int minutos = seg / 60;
    int segundos = seg % 60;
    sprintf(buffer, " %02d:%02d      ", minutos, segundos);  // Mostrar minutos y segundos
    return buffer;
  }
}
// Función para mostrar "Bonus" en la parte inferior izquierda de la pantalla
void mostrarBonus() {
    if (bonus != 0) {
        oledWriteString(&ssoled[1], 0, 0, 7, "bonus", FONT_SMALL, 0, 1);
    }
}

// Función para mostrar movimientos
void mostrarMovimientos(SSOLED *oled, int movimientos) {
    oledWriteString(oled, 0, 0, 0, (char *)("Moves: " + variableToString(movimientos)).c_str(), FONT_NORMAL, 0, 1);
}

void refrescaDisplay(SSOLED *pantalla, unsigned long segunds, int mov) {
    mostrarTiempoRestante(pantalla, segunds);  // Mostrar tiempo restante
    mostrarBonus();  // Mostrar "Bonus" en la pantalla del Jugador 1
    mostrarMovimientos(pantalla, mov);  // Mostrar movimientos del Jugador 1
}

void loop() {
    clock = millis();
    segundos = clock / 1000;

    if (clock > (ultimoclock + 999)) {
        ultimoclock = clock;

        switch (player) {
        case 0:
            oledWriteString(&ssoled[0], 0, 85, 7, (char *)"Play ", FONT_NORMAL, 0, 1);
            if (segundosJugador[0] > 0) {
                segundosJugador[0]--;
            }
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

        for (int i = 0; i < 2; i++) {
            refrescaDisplay(&ssoled[i], segundosJugador[i], moves[i]);
        }
    }

    // Detectar el botón del Jugador 1
    if (digitalRead(2) == LOW && !botonPresionado[0] && (player == 0 || player == PAUSE)) {  
        player = 1;  // Cambia al Jugador 2
        segundosJugador[0] += bonus;  // Agregar bonus al Jugador 2
        moves[0]++;  // Incrementar movimientos del Jugador 1
        botonPresionado[0] = true;
        Serial.println("Cambio a Jugador 2");
    } else {
        botonPresionado[0] = false;
    }
  
    // Detectar el botón del Jugador 2
    if (digitalRead(3) == LOW && !botonPresionado[1] && (player == 1 || player == PAUSE)) {  
        player = 0;  // Cambia al Jugador 1
        segundosJugador[1] += bonus;  // Agregar bonus al Jugador 1
        moves[1]++;  // Incrementar movimientos del Jugador 2
        botonPresionado[1] = true;
        Serial.println("Cambio a Jugador 1");
    } else {
        botonPresionado[1] = false;
    }

    // Detectar el botón del PAUSE
    if (digitalRead(5) == LOW) {
        player = PAUSE;
        Serial.println("PAUSE");
    }
    if (digitalRead(4) == LOW) {
      timeSetting(1);
    }
}
