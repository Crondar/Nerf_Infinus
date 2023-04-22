#include "shared.h"

size_t println(Adafruit_SSD1306 *display, const String &s) {
  size_t result = 0;//Serial.println(s);
  display->clearDisplay();
  display->setCursor(0, 0);
  display->setTextSize(1);               // Normal 1:1 pixel scale
  display->setTextColor(SSD1306_WHITE);  // Draw white text
  display->println(s);
  display->display();
  return result;
}

size_t println(Adafruit_SSD1306 *display, int row, const String &s) {
  size_t result = 0; //Serial.println(s);
  display->clearDisplay();
  display->setCursor(0, row);
  display->setTextSize(1);               // Normal 1:1 pixel scale
  display->setTextColor(SSD1306_WHITE);  // Draw white text
  display->println(s);
  display->display();
  return result;
}

size_t printlnWithXY(Adafruit_SSD1306 *display, const String &s, int x, int y) {
  size_t result = 0; // Serial.println(s);
  display->clearDisplay();
  display->setCursor(x, y);
  display->setTextSize(1);               // Normal 1:1 pixel scale
  display->setTextColor(SSD1306_WHITE);  // Draw white text
  display->println(s);
  display->display();
  return result;
}