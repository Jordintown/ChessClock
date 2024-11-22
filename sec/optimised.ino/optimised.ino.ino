void setup() {
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
  //oledFill(&ssoled[0], 0, 1);
  //oledFill(&ssoled[1], 0, 1);
  digitalWrite(7, HIGH);
  delay(25);
  digitalWrite(7, LOW);
}

void loop() {
if (digitalRead(2)== HIGH){
  Serial.println("player left");
} if (digitalRead(3)== HIGH){
  Serial.println("player left");
} if (digitalRead(4)== HIGH){
Serial.println("+");
} if (digitalRead(5)==HIGH){
  Serial.println("pause");
} if (digitalRead(6)==HIGH){
  Serial.println("-");
}

}
