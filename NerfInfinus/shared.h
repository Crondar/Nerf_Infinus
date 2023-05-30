#pragma once

#include <stddef.h>
#include <Adafruit_SSD1306.h>

size_t println(Adafruit_SSD1306 *display, const String &s);
size_t println(Adafruit_SSD1306 *display, int row, const String &s);
size_t printlnWithXY(Adafruit_SSD1306 *display, const String &s, int x, int y);