/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#define SCREEN_WIDTH 127 // OLED display width, in pixels
//#define SCREEN_HEIGHT 1 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
display.clearDisplay();

display.setTextSize(1);
display.setTextColor(WHITE);
//display.setCursor(4, 4);

//Display static text
display.println("Hello, world!\n");
display.display(); 
}

void loop() {
  //Display static text
display.println("Hello, world!");
display.display(); 
  delay(2000);

}
