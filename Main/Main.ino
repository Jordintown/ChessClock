// ######################
// #   DME CHESS CLOCK  #
// #     FOR ARDUINO    #
// #     PART OF THE    #
// #  ARDUCHESS PROJECT #
// ######################
// https://github.com/Jordintown/ChessClock

#include <ss_oled.h>
#include <EEPROM.h>
//#include <Arduino.h>
#include "conversions.h"
#include "config.h"

//static code legibility defs
#define PAUSE 3
#define TIMEOUT 4

SSOLED ssoled[2];

uint8_t LDUbuffer[1024];
uint8_t RDUbuffer[1024];

//unsigned long milisegundos;
unsigned long reloj, ultimoclock, millisMax;
unsigned long segundosJugador[2];
//unsigned long BigClock = 0;
unsigned int bonus;
unsigned short moves[2];
unsigned short player = PAUSE;
unsigned short dVbat[SZ_MUESTREO_BAT], muestraBat;
//unsigned int gamePhase[2];
int boton=0;
bool botonPresionado[2],  link = false;
//temp declarations to be moved somewhere else
/*unsigned short phaseTrigger[2];
unsigned int phaseTime[2];
unsigned short phaseBonus[2];*/


struct persistente {
  unsigned long tiempo;
  unsigned int bonus;
  unsigned short beep;
 /*int phaseTrigger[2];
 long phaseTime[2];
 int phaseBonus[2];*/
};

persistente persist;

void setup() {
  digitalWrite(LED_BUILTIN, HIGH);
  oledSetBackBuffer(&ssoled[0], LDUbuffer);
  oledSetBackBuffer(&ssoled[1], RDUbuffer);
  oledInit(&ssoled[0], OLED_128x64, -1, 0, 0, 1, A5, A4, -1, 400000L);
  oledInit(&ssoled[1], OLED_128x64, -1, 0, 0, 0, A2, A3, -1, 400000L);
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
  dibujaLogo();
  oledWriteString(&ssoled[1], 0, 0, 3, (char *)("Self test in progress"), FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[1], 0, 20, 4, (char *)("(Max. 3 seconds)"), FONT_SMALL, 0, 1);
  if (ALLOW_BOOT!=1){
    error("BOOT ERROR", "The configuration", "data is not valid", " ", "", "CNF_BOOT_DISBLD", false);
    //error("DME ChessClock", "An open source", "Arduino Chess Clock", "with great features", "WLCM_MSSG_DSPLY", false);
  }
  for(muestraBat=0;muestraBat<SZ_MUESTREO_BAT;muestraBat++){
    dVbat[muestraBat]=INIT_BAT_dV;
  }

  Serial.begin(115200);

  EEPROM.get(0, persist);

  /*phaseTrigger[0]=persist.phaseTrigger[0];
  phaseTrigger[1]=persist.phaseTrigger[1];
  phaseTime[0]=persist.phaseTime[0];
  phaseTime[1]=persist.phaseTime[1];
  phaseBonus[0]=persist.phaseBonus[0];
  phaseBonus[1]=persist.phaseBonus[1];*/

  for (int i = 0; i < 2; i++) {
    segundosJugador[i] = persist.tiempo;
    bonus = persist.bonus;
    moves[i] = 0;
    botonPresionado[i] = 0;
    oledFill(&ssoled[i], 0, 1);
    //gamePhase[i]=1;
  }
  ultimoclock = 0;
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

  digitalWrite(LED_BUILTIN, LOW);
  if ((persist.beep) == 1) {
    digitalWrite(7, HIGH);
    delay(25);
    digitalWrite(7, LOW);
  }
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
  digitalWrite(LED_TX, HIGH);
  timeSetting(0);
}

void error( char* title, char* l1, char* l2, char* l3,char* l4, char* code, bool skippable){
  char buffer[32];
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
  oledSetBackBuffer(&ssoled[0], LDUbuffer);
  oledSetBackBuffer(&ssoled[1], RDUbuffer);
  if (skippable==true){
    while (digitalRead(5)==LOW){
      oledWriteString(&ssoled[0], 0, 7, 1, (char *)title, FONT_NORMAL, 1, 0);
      oledWriteString(&ssoled[0], 0, 10, 3, (char *)l1, FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[0], 0, 10, 4, (char *)l2, FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[0], 0, 10, 5, (char *)l3, FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[0], 0, 10, 6, (char *)l4, FONT_SMALL, 0, 0);
      oledRectangle(&ssoled[0], 6, 7, 124, 16, 1, 0);
      oledRectangle(&ssoled[0], 6, 7, 124, 62, 1, 0);
      oledDumpBuffer(&ssoled[0], (NULL));
      //
      oledWriteString(&ssoled[1], 0, 7, 1, (char *)"CLOCK DATA", FONT_NORMAL, 1, 0);
      sprintf(buffer, "FIRMWARE: v%s", FIRM_VER);
      oledWriteString(&ssoled[1], 0, 10, 3, (char *)buffer, FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[1], 0, 10, 4, (char *)"ERROR CODE:", FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[1], 0, 10, 5, (char *)code, FONT_SMALL, 0, 0);
      oledRectangle(&ssoled[1], 6, 7, 124, 16, 1, 0);
      oledRectangle(&ssoled[1], 6, 7, 124, 62, 1, 0);
      oledRectangle(&ssoled[1], 6, 55, 124, 62, 1, 0);
      oledWriteString(&ssoled[1], 0, 7, 7, (char *)"PAUSE: continue", FONT_NORMAL, 1, 0);
      oledDumpBuffer(&ssoled[1], (NULL));
    }
  } else {
    while (1){
      oledWriteString(&ssoled[0], 0, 7, 1, (char *)title, FONT_NORMAL, 1, 0);
      oledWriteString(&ssoled[0], 0, 10, 3, (char *)l1, FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[0], 0, 10, 4, (char *)l2, FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[0], 0, 10, 5, (char *)l3, FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[0], 0, 10, 6, (char *)l4, FONT_SMALL, 0, 0);
      oledRectangle(&ssoled[0], 6, 7, 124, 16, 1, 0);
      oledRectangle(&ssoled[0], 6, 7, 124, 62, 1, 0);
      oledDumpBuffer(&ssoled[0], (NULL));
      //
      oledWriteString(&ssoled[1], 0, 7, 1, (char *)"CLOCK DATA", FONT_NORMAL, 1, 0);
      sprintf(buffer, "FIRMWARE: v%s", FIRM_VER);
      oledWriteString(&ssoled[1], 0, 10, 3, (char *)buffer, FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[1], 0, 10, 4, (char *)"ERROR CODE:", FONT_SMALL, 0, 0);
      oledWriteString(&ssoled[1], 0, 10, 5, (char *)code, FONT_SMALL, 0, 0);
      oledRectangle(&ssoled[1], 6, 7, 124, 16, 1, 0);
      oledRectangle(&ssoled[1], 6, 7, 124, 62, 1, 0);
      oledDumpBuffer(&ssoled[1], (NULL));
    }
  }
  oledFill(&ssoled[0], 0, 1);
  oledFill(&ssoled[1], 0, 1);
}

/*void procesarEntrada(String entrada) {
  if (entrada.startsWith("SetTime")) { // Verifica si el comando es SetTime
    entrada.remove(0, 8); // Elimina "SetTime " del principio
    int indices[8]; // Índices para los valores separados por comas

    // Extraer los valores
    for (int i = 0; i < 8; i++) {
      int comaIndex = entrada.indexOf(',');
      String valor = (comaIndex == -1) ? entrada : entrada.substring(0, comaIndex);
      indices[i] = valor.toInt();
      entrada = (comaIndex == -1) ? "" : entrada.substring(comaIndex + 1);
    }

    // Asignar a variables
    persist.tiempo = indices[0];
    segundosJugador[0]=persist.tiempo;
    segundosJugador[1]=persist.tiempo;
    bonus = persist.bonus;
    persist.bonus = indices[1];
    persist.phaseTrigger[0] = indices[2];
    persist.phaseTime[0] = indices[3];
    persist.phaseBonus[1] = indices[4];
    persist.phaseTrigger[1] = indices[5];
    persist.phaseTime[1] = indices[6];
    persist.phaseBonus[1] = indices[7];
    EEPROM.put(0, persist);
  }
}*/

void dibujaReloj(SSOLED &display){
  oledRectangle(&display, 25, 5, 35, 6, 1, 1);
  oledRectangle(&display, 29, 5, 31, 12, 1, 0);
  oledRectangle(&display, 30, 37, 30, 18, 1, 1);
  oledEllipse(&display, 30, 37, 25, 25, 1, 0);
  oledDumpBuffer(&display, (LDUbuffer));
}

void dibujaLogo(){
  oledFill(&ssoled[0], 0, 1);
  oledSetBackBuffer(&ssoled[0], LDUbuffer);
  oledRectangle(&ssoled[0], 25, 5, 35, 6, 1, 1);
  oledRectangle(&ssoled[0], 29, 5, 31, 12, 1, 0);
  oledRectangle(&ssoled[0], 30, 37, 30, 18, 1, 1);
  oledEllipse(&ssoled[0], 30, 37, 25, 25, 1, 0);
  oledDumpBuffer(&ssoled[0], (NULL));
  //while (1);
}

void beep(long time) {
  if ((persist.beep) == 1) {
    millisMax = millis() + time;
    digitalWrite(7, HIGH);
  }
}

void muestreoBat(){
  if(muestraBat>=SZ_MUESTREO_BAT){
    muestraBat=0;
  }
  dVbat[muestraBat++]=(int)((analogRead(0) + 80.) / 10.2);
}

float consultaBatV(){
  unsigned long acumulador=0;
  for(int i=0;i<SZ_MUESTREO_BAT;i++){
    acumulador+=dVbat[i];
  }
 // Serial.println(((float)(acumulador/TAM_MUESTREO_BAT))/10.0);
  return ((float)(acumulador/SZ_MUESTREO_BAT))/10.0;
}

float consultaBatPercent(){
  float resultat=(consultaBatV() - 6.6) * 62.5;
  if (resultat < 0.) { resultat = 0.; }
  if (resultat >= 100.) { resultat = 100.; }
  return resultat;
  //(volts - 6.6) * 62.5
}

void timeSetting(int type) {
  char buf[15];
  if (type == 0) {  // I" "nitial setting when starting the clock
    oledFill(&ssoled[0], 0, 1);
    oledFill(&ssoled[1], 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 0, (char *)("Confirm time settings"), FONT_SMALL, 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 1, (char *)("retrieved from memory"), FONT_SMALL, 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 7, (char *)("PAUSE: confirm"), FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 6, (char *)("+/-: modify"), FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[1], 0, 30, 0, (char *)("Time/Bonus"), FONT_SMALL, 0, 1);
    ToHMS(segundosJugador[0], buf);
    oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
    ToHMS(bonus, buf);
    oledWriteString(&ssoled[1], 0, 0, 5, (char *)buf, FONT_STRETCHED, 0, 1);

    while (1) {
      if (digitalRead(5) == LOW) {
        break;
      }
      if (digitalRead(4) == LOW) {
        timeSetting(1);
      }
      if (digitalRead(6) == LOW) {
        timeSetting(1);
      }
      if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        if (command == "LINK LOGON") {
          // Send LOGON-CONFIRM back to the Python side
          Serial.println("LOGON CONFIRM");
          link = true;
          break;
        }
      }
    }
    oledFill(&ssoled[0], 0, 1);
    oledFill(&ssoled[1], 0, 1);
  } else if (type == 1) {
    player = PAUSE;
    int mod = 0;
    oledFill(&ssoled[0], 0, 1);
    oledFill(&ssoled[1], 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 0, (char *)("Default time settings"), FONT_SMALL, 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 7, (char *)("PAUSE: next/conf."), FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 6, (char *)("+/-: modify"), FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[1], 0, 30, 0, (char *)("Time/Bonus"), FONT_SMALL, 0, 1);
    ToHMS(persist.tiempo, buf);
    oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
    ToHMS(persist.bonus, buf);
    oledWriteString(&ssoled[1], 0, 0, 5, (char *)buf, FONT_STRETCHED, 0, 1);
    while (1) {
      if (digitalRead(4) == LOW) {
        if (mod == 0) {
          if (persist.tiempo>=600){
            persist.tiempo += 600;
          } else if (persist.tiempo<600)
          {
            persist.tiempo += 100;
          }
        } else if (mod == 1) {
          persist.bonus++;
        }
      }
      if (digitalRead(5) == LOW) {
        if (mod == 0) {
          mod++;
        } else if (mod == 1) {
          break;
        } else {
          Serial.println("ERROR: parametro incorrecto");
        }
      }
      if (digitalRead(6) == LOW) {
        if (mod == 0) {
          if (persist.tiempo>=600){
            persist.tiempo -= 600;
          } else if (persist.tiempo<600)
          {
            persist.tiempo -= 100;
          }
        } else if (mod == 1) {
          persist.bonus--;
        }
      }
      if (mod==0){
        ToHMS(persist.tiempo, buf);
        oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 1, 1);
        ToHMS(persist.bonus, buf);
        oledWriteString(&ssoled[1], 0, 0, 5, (char *)buf, FONT_STRETCHED, 0, 1);
      } if (mod==1){
        ToHMS(persist.tiempo, buf);
        oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
        ToHMS(persist.bonus, buf);
        oledWriteString(&ssoled[1], 0, 0, 5, (char *)buf, FONT_STRETCHED, 1, 1);
      }
    }
    oledFill(&ssoled[0], 0, 1);
    oledFill(&ssoled[1], 0, 1);
    segundosJugador[0] = persist.tiempo;
    segundosJugador[1] = segundosJugador[0];
    bonus = persist.bonus;
    EEPROM.put(0, persist);
    EEPROM.put(0, persist);
    mod = 0;
    moves[0] = 0;
    moves[1] = 0;
  } else if (type == 2) {
    int mod = 0;
    int modif[2] = {0, 0};
    String texto;
    player = PAUSE;
    oledFill(&ssoled[0], 0, 1);
    oledFill(&ssoled[1], 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 0, (char *)("Current game modif."), FONT_SMALL, 0, 1);
    oledWriteString(&ssoled[1], 0, 0, 0, (char *)("Add or remove time"), FONT_SMALL, 0, 1);
    oledWriteString(&ssoled[1], 0, 0, 7, (char *)("PAUSE: next/conf."), FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[0], 0, 0, 7, (char *)("+/-: modify"), FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled[0], 0, 30, 5, (char *)("Player 1"), FONT_SMALL, 0, 1);
    oledWriteString(&ssoled[1], 0, 30, 5, (char *)("Player 2"), FONT_SMALL, 0, 1);

    texto = String("   " + String(modif[0]) + "   ");
    texto.toCharArray(buf, 15);
    oledWriteString(&ssoled[0], 0, 50, 3, buf, FONT_STRETCHED, 0, 1);

    texto = String("   " + String(modif[1]) + "   ");
    texto.toCharArray(buf, 15);
    oledWriteString(&ssoled[1], 0, 50, 3, buf, FONT_STRETCHED, 0, 1);

    while (1) {
      delay(75);
      if (digitalRead(4) == LOW) {
        if (mod == 0) {
          modif[0] += 10;
        } else if (mod == 1) {
          modif[1] += 10;
        }
      }
      if (digitalRead(5) == LOW) {
        if (mod == 0) {
          mod++;
        } else if (mod == 1) {
          break;
        } else {
          Serial.println("ERROR: parametro incorrecto");
        }
      }
      if (digitalRead(6) == LOW) {
        if (mod == 0) {
          modif[0] -= 10;
        } else if (mod == 1) {
          modif[1] -= 10;
        }
      }

      if (mod == 0) {
        texto = String("   " + String(modif[0]) + "    ");
        texto.toCharArray(buf, 15);
        oledWriteString(&ssoled[0], 0, 0, 3, buf, FONT_STRETCHED, 1, 1);

        texto = String("   " + String(modif[1]) + "    ");
        texto.toCharArray(buf, 15);
        oledWriteString(&ssoled[1], 0, 0, 3, buf, FONT_STRETCHED, 0, 1);
      }
      if (mod == 1) {
        texto = String("   " + String(modif[0]) + "    ");
        texto.toCharArray(buf, 15);
        oledWriteString(&ssoled[0], 0, 0, 3, buf, FONT_STRETCHED, 0, 1);

        texto = String("   " + String(modif[1]) + "    ");
        texto.toCharArray(buf, 15);
        oledWriteString(&ssoled[1], 0, 0, 3, buf, FONT_STRETCHED, 1, 1);
      }
    }

    segundosJugador[0] += 10 * modif[0];
    segundosJugador[1] += 10 * modif[1];
    oledFill(&ssoled[0], 0, 1);
    oledFill(&ssoled[1], 0, 1);
    mod = 0;
  } else {
    Serial.println("ERROR: parametro incorrecto");
  }
  for (int i = 0; i < 2; i++) {
    refrescaDisplay(&ssoled[i], segundosJugador[i]);
  }
}


// Función para convertir segundos a hh:mm o mm:ss
void mostrarTiempoRestante(SSOLED *oled, unsigned long segundosRestantes) {
  char buffer[15];
  int decimasRestantes = segundosRestantes % 10;
  segundosRestantes = segundosRestantes / 10;
  if (segundosRestantes >= 3600) {  // Si hay más de 1 hora
    int horas = segundosRestantes / 3600;
    int minutos = (segundosRestantes % 3600) / 60;
    int segundos = segundosRestantes % 60;
    sprintf(buffer, "%2d:%02d:%02d", horas, minutos, segundos);  // Mostrar solo horas y minutos
    oledWriteString(oled, 0, 0, 3, buffer, FONT_STRETCHED, 0, 1);
  } else if (segundosRestantes < DEC_TRESHOLD) {
    int minutos = (segundosRestantes % 3600) / 60;
    int segundos = segundosRestantes % 60;
    sprintf(buffer, "0:%02d.%d", segundos, decimasRestantes);  // Mostrar solo horas y minutos
    oledWriteString(oled, 0, 18, 3, buffer, FONT_STRETCHED, 0, 1);
  } else {  // Menos de 1 hora
    int minutos = segundosRestantes / 60;
    int segundos = segundosRestantes % 60;
    sprintf(buffer, "%02d:%02d   ", minutos, segundos);  // Mostrar minutos y segundos
    oledWriteString(oled, 0, 18, 3, buffer, FONT_STRETCHED, 0, 1);
  }
}

// Función para mostrar "Bonus" en la parte inferior izquierda de la pantalla
void mostrarBonus() {
  char buf[25];
  int inc = bonus;
  if (bonus != 0) {
    String texto = String((int)inc);
    texto = "Bonus +" + texto + "";
    texto.toCharArray(buf, 25);
    oledWriteString(&ssoled[1], 0, 0, 0, buf, FONT_SMALL, 0, 1);
  } else {
    oledWriteString(&ssoled[1], 0, 0, 0, "         ", FONT_SMALL, 0, 1);
  }
}

// Función para mostrar movimientos
void mostrarEstado() {

  oledWriteString(&ssoled[0], 0, 93, 6, (char *)"Moves:", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[0], 0, 100, 7, (char *)variableToString(moves[0]).c_str(), FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[1], 0, 0, 6, (char *)"Moves:", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[1], 0, 0, 7, (char *)variableToString(moves[1]).c_str(), FONT_NORMAL, 0, 1);
  //
  /*oledWriteString(&ssoled[0], 0, 53, 6, (char *)"Phase:", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[0], 0, 53, 7, (char *)variableToString(gamePhase[0]).c_str(), FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled[1], 0, 40, 6, (char *)"Phase:", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled[1], 0, 40, 7, (char *)variableToString(gamePhase[1]).c_str(), FONT_NORMAL, 0, 1);*/
}

void refrescaDisplay(SSOLED *pantalla, unsigned long segunds) {
  char buf[20];
  unsigned short rmk;
    if (player!=TIMEOUT){
      mostrarTiempoRestante(pantalla, segunds);  // Mostrar tiempo restante
    }
    if (player==PAUSE || TIMEOUT){
      mostrarBonus();                            // Mostrar "Bonus" en la pantalla del Jugador 1
    }
    mostrarEstado();                      // Mostrar movimientos del Jugador 1
  if (persist.beep == 1) {
    oledWriteString(&ssoled[0], 0, 0, 0, "         ", FONT_SMALL, 0, 1);
  }
  if (persist.beep == 0) {
    oledWriteString(&ssoled[0], 0, 0, 0, "Sound off", FONT_SMALL, 0, 1);
  }
  if (link == true) {
    digitalWrite(LED_TX, LOW);
    oledWriteString(&ssoled[1], 0, 60, 0, "  ChessLink", FONT_SMALL, 0, 1);
  } else {
    digitalWrite(LED_TX, HIGH);
    oledWriteString(&ssoled[1], 0, 60, 0, "           ", FONT_SMALL, 0, 1);
  }

  muestreoBat();
  //      if ((2*(analogRead(0)*(5.0/1024.0))) < 5.2){
  //  float volts = ((analogRead(0) + 80) / 102.);
  //  float percent = (volts - 6.6) * 62.5;

}

void displayBat(){
  char buf[15];
  unsigned short rmk;
  float percent=consultaBatPercent();
  String texto = String((int)percent);
  texto = "Bat: " + texto + "%  ";
  texto.toCharArray(buf, 15);
  if (percent<BAT_WARN_TRESH) rmk =1;
  else rmk =0;
  if (percent>0){
    oledWriteString(&ssoled[0], 0, 70, 0, buf, FONT_SMALL, rmk, 1);
  } else if (percent==0) {
    oledWriteString(&ssoled[0], 0, 70, 0, "          ", FONT_SMALL, 0, 1);
  }
  //Serial.println(percent);
}
void SerialOvrd() {
  if (link == true) {
    Serial.print(segundosJugador[0]);
    Serial.print(",");
    Serial.print(moves[0]);
    Serial.print(",");
    Serial.print(segundosJugador[1]);
    Serial.print(",");
    Serial.println(moves[1]);
  }
}
void ToggleBeep() {
      if (persist.beep == 0) {
        persist.beep = 1;
        EEPROM.put(0, persist);
        for (int i = 0; i < 2; i++) {
          digitalWrite(7, HIGH);
          delay(25);
          digitalWrite(7, LOW);
          delay(25);
        }
      } else if (persist.beep == 1) {
        persist.beep = 0;
        EEPROM.put(0, persist);
      } else {
        Serial.println("ERROR: incorrect value, returning to known state (beep on)");
        persist.beep = 1;
      }
}

void dibujaBandera(){
  oledSetBackBuffer(&ssoled[0], LDUbuffer);
  oledRectangle(&ssoled[0], 10, 15, 30, 25, 1, 1);
  oledRectangle(&ssoled[0], 12, 25, 40, 45, 0, 1);
  oledEllipse(&ssoled[0], 16, 15, 4, 2, 0, 1);
  oledEllipse(&ssoled[0], 25, 25, 4, 2, 0, 1);
  oledEllipse(&ssoled[0], 25, 15, 4, 2, 1, 1);
  oledEllipse(&ssoled[0], 16, 25, 4, 2, 1, 1);
  oledRectangle(&ssoled[0], 10, 15, 11, 45, 1, 1);
  oledDumpBuffer(&ssoled[0], NULL);
}

void DibujaBandera(){
  int ofst=90;
  oledSetBackBuffer(&ssoled[1], RDUbuffer);
  oledRectangle(&ssoled[1], 10+ofst, 15, 30+ofst, 25, 1, 1);
  oledRectangle(&ssoled[1], 12+ofst, 25, 40+ofst, 45, 0, 1);
  oledEllipse(&ssoled[1], 16+ofst, 15, 4, 2, 0, 1);
  oledEllipse(&ssoled[1], 25+ofst, 25, 4, 2, 0, 1);
  oledEllipse(&ssoled[1], 25+ofst, 15, 4, 2, 1, 1);
  oledEllipse(&ssoled[1], 16+ofst, 25, 4, 2, 1, 1);
  oledRectangle(&ssoled[1], 10+ofst, 15, 11+ofst, 45, 1, 1);
  oledDumpBuffer(&ssoled[1], NULL);
}

int botonPulsado(){  //  Trato de controlar pulsaciones multiples de boton
  int devolver=boton;//  Almacenamos el ultimo boton pulsado. Devolveremos su valor si ya se ha soltado.
  if(boton==0){      //  Si hemos detectado una pulsación solo tmparemos nota de ello en la variable global boton
    for(int i=6;i>=2;i--){
      if(digitalRead(i)==LOW){
        boton=i;
      }
    }
  }
  else{
    boton=0;          // si había un boton pulsado, vamos escanear a a ver si se ha soltado.
    for(int i=6;i>=2;i--){
      if(digitalRead(i)==LOW){
        boton=i;
      }
    }
    if(boton!=0){     // Si hemos detectado algo pulsado, todavía no vamos a dar la pulsacion por detectada.
      devolver=0;
    }
  }
  return devolver;    // Solo devolveremos algo distinto de 0 cuando en la ultima ejecución había algo pulsado pero en esta no.
}

void CambiaEstado(){ // cambia el estado del reloj segun el boton pulsado
  switch (botonPulsado()){
    case 2:           // Detectado P1
    switch (player){
      case PAUSE:     // estamos en pausa

          SerialOvrd();
          oledWriteString(&ssoled[0], 0, 0, 6, (char *)" ", FONT_STRETCHED, 0, 1);
          oledWriteString(&ssoled[1], 0, 110, 6, (char *)">", FONT_STRETCHED, 0, 1);


        player=1;
      break;
      case 0:         // turno de P1

          //SerialOvrd();
            segundosJugador[0] += (10 * bonus);  // Agregar bonus al Jugador 1
            moves[0]++;                        // Incrementar movimientos del Jugador 1
          oledWriteString(&ssoled[0], 0, 0, 6, (char *)" ", FONT_STRETCHED, 0, 1);
          oledWriteString(&ssoled[1], 0, 110, 6, (char *)">", FONT_STRETCHED, 0, 1);
          refrescaDisplay(&ssoled[0], segundosJugador[0]);
          //phaseWork(0);
        player=1;
      break;
    }
    break;
    case 3:           // Detectado P2
    switch (player){
      case PAUSE:     // estamos en pausa
          oledWriteString(&ssoled[0], 0, 0, 6, (char *)"<", FONT_STRETCHED, 0, 1);
          oledWriteString(&ssoled[1], 0, 110, 6, (char *)" ", FONT_STRETCHED, 0, 1);
        player=0;
      break;
      case 1:         // turno de P2

          //SerialOvrd();
            segundosJugador[1] += (10 * bonus);    // agregar bonus a 2
            moves[1]++;                          // Incrementar movimientos del Jugador 2
          oledWriteString(&ssoled[0], 0, 0, 6, (char *)"<", FONT_STRETCHED, 0, 1);
          oledWriteString(&ssoled[1], 0, 110, 6, (char *)" ", FONT_STRETCHED, 0, 1);
          refrescaDisplay(&ssoled[1], segundosJugador[1]);
          //phaseWork(1);
        player=0;
      break;
    }
    break;
    case 5:           // Detectado PAUSE
    switch (player){
      case 0:

        oledWriteString(&ssoled[0], 0, 0, 6, (char *)" ", FONT_STRETCHED, 0, 1);
        oledWriteString(&ssoled[1], 0, 110, 6, (char *)" ", FONT_STRETCHED, 0, 1);

        player=PAUSE;
      break;
      case 1:

        oledWriteString(&ssoled[0], 0, 0, 6, (char *)" ", FONT_STRETCHED, 0, 1);
        oledWriteString(&ssoled[1], 0, 110, 6, (char *)" ", FONT_STRETCHED, 0, 1);

        player=PAUSE;
      break;
    }
    break;
    case 4:           // Detectado +
    switch (player){
      case PAUSE:
        timeSetting(1);
      break;
    }
    break;
    case 6:           // Detectado -
    switch (player){
      case PAUSE:
        timeSetting(2);
      break;
    }
    break;
  }
}

/*void phaseWork(unsigned int player){
  if (moves[player]==persist.phaseTrigger[0]){
    segundosJugador[player]+=10*persist.phaseTime[0];
    bonus=persist.phaseBonus[0];
    gamePhase[0]=2;
  } if (moves[player]==persist.phaseTrigger[1]){
    segundosJugador[player]+=10*persist.phaseTime[1];
    bonus=persist.phaseBonus[1];
    gamePhase[1]=3;
  }
refrescaDisplay(&ssoled[player], segundosJugador[player]);
}*/

void serialSys(){
      // unsigned long currentTime = millis();
    String command = Serial.readStringUntil('\n');  // Read incoming command
    //procesarEntrada(command);
    if (command == "LINK LOGON") {
      // Send LOGON-CONFIRM back to the Python side
      Serial.println("LOGON CONFIRM");
      link = true;
    }
    if (command == "DEFAULT") {
      player = PAUSE;
      persist.tiempo = DEF_TIME;
      segundosJugador[0] = DEF_TIME;
      segundosJugador[1] = DEF_TIME;
      persist.bonus = 0;
      persist.beep = 1;
      EEPROM.put(0, persist);
      EEPROM.put(0, persist);
    }
    if (command == "BEEP") {
      ToggleBeep();
      EEPROM.put(0, persist);
    } else if (command == "HELP") {
      Serial.println(("DEFAULT: returns to default values (10+0, beep on)"));
      Serial.println(("BEEP: toggles the buzzer (shows on screen status)"));
      Serial.println(("HELP: shows this"));
    } if (command == "LINK STOP") {
      // Send LOGON-CONFIRM back to the Python side
      link = false;
    } if (command == "FETCH") {
      SerialOvrd();
    } if (command=="FIRUPDATE"){
      error("FIRMWARE UPDATE", "Update in progress", "","DO NOT UNPLUG THE", "CLOCK DURING UPDATE", "FMW_UPDTE", false);
    }
    int player1Time = segundosJugador[0];  // Replace with actual logic
    int player2Time = segundosJugador[1];

    // Send formatted data: player1_time,player2_time
}

void loop() {
  char buf[40];
  reloj = millis();
  CambiaEstado();     //   reacciona a la pulsacion de botones
  // Paramos el beep tras su timeout
  if (reloj > millisMax) {
    digitalWrite(7, LOW);
  }

  if (Serial.available()) {
    serialSys();
  }

  if (reloj >= (ultimoclock + 99)) {
    displayBat();
    ultimoclock = reloj;
    if (DBG_MILLIS_PASS!=0){
      Serial.println(millis());
    }

    switch (player) {
      case 0:
        if (segundosJugador[0] > 0 && ALLOW_COUNTDOWN==1) {
          segundosJugador[0]--;
        }
        if (segundosJugador[0] == 0) {
          player = TIMEOUT;
          beep(1500);
          //digitalWrite(9, HIGH);
          //ToHMS(segundosJugador[1], buf);
          dibujaBandera();
          sprintf(buf, "0:00  ");
          oledWriteString(&ssoled[0], 0, 40, 3, (char *)buf, FONT_STRETCHED, 0, 1);
        }
        if (segundosJugador[0] == 30 || segundosJugador[0] == 20 || segundosJugador[0] == 10) {
          beep(100);
        }
        refrescaDisplay(&ssoled[0], segundosJugador[0]);
        SerialOvrd();
        break;

      case 1:
        if (segundosJugador[1] > 0 && ALLOW_COUNTDOWN==1) {
          segundosJugador[1]--;
        }
        if (segundosJugador[1] == 0) {
          player = TIMEOUT;
          beep(1500);
          //digitalWrite(10, HIGH);
          //ToHMS(segundosJugador[1], buf);
          DibujaBandera();
          oledWriteString(&ssoled[1], 0, 20, 3, (char *)"0:00", FONT_STRETCHED, 0, 1);
        }
        if (segundosJugador[1] == 30 || segundosJugador[1] == 20 || segundosJugador[1] == 10) {
          beep(100);
        }
        refrescaDisplay(&ssoled[1], segundosJugador[1]);
        SerialOvrd();
        break;

      case PAUSE:
        refrescaDisplay(&ssoled[0], segundosJugador[0]);
        refrescaDisplay(&ssoled[1], segundosJugador[1]);
        oledWriteString(&ssoled[0], 0, 0, 6, (char *)" ", FONT_STRETCHED, 0, 1);
        oledWriteString(&ssoled[1], 0, 110, 6, (char *)" ", FONT_STRETCHED, 0, 1);
        break;

      case TIMEOUT:
        refrescaDisplay(&ssoled[0], segundosJugador[0]);
        refrescaDisplay(&ssoled[1], segundosJugador[1]);
        oledWriteString(&ssoled[0], 0, 0, 6, (char *)" ", FONT_STRETCHED, 0, 1);
        oledWriteString(&ssoled[1], 0, 110, 6, (char *)" ", FONT_STRETCHED, 0, 1);
        break;

      default:
        oledWriteString(&ssoled[0], 0, 60, 7, (char *)"ERROR", FONT_NORMAL, 0, 1);
        Serial.println("Error: estado desconocido");
    }
  }

}
