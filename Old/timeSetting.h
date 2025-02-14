void timeSetting(int type) {
  char buf[15];
  if (type == 0) {
  } else if (type == 1) {
    player = PAUSE;
    int mod = 0;
    oledFill(&ssoled[0], 0, 1);
    oledFill(&ssoled[1], 0, 1);
  
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
