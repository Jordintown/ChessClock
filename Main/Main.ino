
unsigned long clock,segundos,ultimoclock,segundos1,segundos2;


int jugador1=0;


void setup(){
  Serial.begin(9600);
  ultimoclock=0;
  segundos1=0;
  segundos2=0;
  
}

void loop(){
  clock = millis();
  segundos = clock/1000;
  if (Serial.available() > 0) {
        Serial.print((char)Serial.read());
        jugador1=!jugador1;
  }
  if(clock>(ultimoclock+999)){
    ultimoclock=clock;
    Serial.print(String(segundos)+" | "+String(segundos));
    if(jugador1) Serial.println(" <");
    else Serial.println(" >");
  }

} 