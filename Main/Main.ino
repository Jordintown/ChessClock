
unsigned long clock,segundos,ultimoclock,segundos1,segundos2;

void setup(){
  Serial.begin(9600);
  ultimoclock=0;
  segundos1=500;
  segundos2=500;
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);

}

void loop(){
  int moves1=0;
  int moves2=0;
  int jugador1;
  
  clock = millis();
  segundos = clock/1000;
  if (Serial.available() > 0) {
        Serial.print((char)Serial.read());
        jugador1=!jugador1;
  }
  if(clock>(ultimoclock+999)){
    ultimoclock=clock;
    
    if(jugador1=0){
      segundos1--;
      moves1++;
      Serial.println(String(segundos1)+" < "+String(segundos2))+"\n";
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);  
        if (segundos1<1);{

      }
    } 
    else{
      segundos2--;
      moves2++;
      Serial.println(String(segundos1)+" > "+String(segundos2))+"\n";
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);  
      if (segundos2<1);{

      }
    } 
  }

} 