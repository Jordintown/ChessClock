#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,A5,A4,U8X8_PIN_NONE);

void setup(void) {
  u8g2.begin();
}

void loop(void) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5,10,"ideaspark 0.96'' OLED");
  u8g2.drawStr(5,25,"0.96");
  u8g2.drawStr(5,35,"0.96 inch");
  u8g2.drawStr(5,45,"0.96 inch OLED");
  u8g2.drawStr(5,55,"0.96 inch OLED 128x64");
  u8g2.sendBuffer();
  delay(1000);
}