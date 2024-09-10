#include <OLED_I2C.h>
#include <math.h>
OLED  myOLED(SDA, SCL, 8);
extern uint8_t SmallFont[];// Selecciono el tamaño de fuente (Small o Big)
extern uint8_t MediumNumbers[];// Selecciono el tamaño de fuente para los numeros

void setup()
{
  myOLED.begin();//inicializa el display OLED
  myOLED.setFont(SmallFont);//seteo el tamaño de la fuente
  myOLED.print("OLED 128x64", CENTER, 0);//imprime la frase entre comillas
  myOLED.update();// actualiza la pantalla haciendo lo anterior
  delay(1000);
  myOLED.clrScr(); // borra la pantalla
}

void loop () 
{
  int i=0;
  int j=0;
  for(i=0;i<64;i++){
  myOLED.printNumI(j,j-12,20);// imprime el valor de la entrada analogica(valor,x,y)
  myOLED.printNumI(i,1,i-8);// imprime el valor de la entrada analogica
  myOLED.drawLine(1,i,30,i);// dibuja una recta entre (X0,Y0) y (X1,Y1) seteando (X0,Y0,X1,Y1)
  myOLED.drawLine(j,10,j,30);// dibuja una recta entre (X0,Y0) y (X1,Y1) seteando (X0,Y0,X1,Y1)
  myOLED.update();//actualiza la pantalla
  delay(100);
  myOLED.clrScr(); //borra la pantalla 
  j=j+1;
  }
  delay(500);
  for(j=63;j<128;j++){
  myOLED.printNumI(j,j-17,20);// imprime el valor de la entrada analogica(valor,x,y)
  myOLED.printNumI(63,1,i-8);// imprime el valor de la entrada analogica
  myOLED.drawLine(1,63,30,63);// dibuja una recta entre (X0,Y0) y (X1,Y1) seteando (X0,Y0,X1,Y1)
  myOLED.drawLine(j,10,j,30);// dibuja una recta entre (X0,Y0) y (X1,Y1) seteando (X0,Y0,X1,Y1)
  myOLED.update();//actualiza la pantalla
  delay(100);
  myOLED.clrScr(); //borra la pantalla 
  }
  myOLED.printNumI(j,j-21,20);// imprime el valor de la entrada analogica(valor,x,y)
  delay(3000);
}