char *ToHMS(long seg, char *buffer) {
  //char buffer[15];
  if (seg >= 36000) {  // Si hay más de 1 hora
    int horas = seg / 36000;
    int minutos = (seg % 36000) / 600;
    int segundos = seg % 6;
    sprintf(buffer, "%2d:%02d:%02d   ", horas, minutos, segundos);  // Mostrar solo horas y minutos
    return buffer;
  } else {  // Menos de 1 hora
    int minutos = seg / 600;
    int segundos = seg % 60;
    sprintf(buffer, " %02d:%02d      ", minutos, segundos);  // Mostrar minutos y segundos
    return buffer;
  }
}

String variableToString(int value) {
  return String(value);
}

String variableToString(unsigned int value) {
  return String(value);
}

String variableToString(long value) {
  return String(value);
}

String variableToString(unsigned long value) {
  return String(value);
}

char* intToChar(int num) {
    // Calcula el espacio necesario (incluye el carácter nulo)
    char* str = (char*)malloc(12);  // Suficiente para un int de 32 bits (+ signo y null)
    if (str != NULL) {
        sprintf(str, "%d", num);  // Convierte el int a char*
    }
    return str;
}