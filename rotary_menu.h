#pragma once
#include "Adafruit_seesaw.h"
#include <Adafruit_SSD1306.h>
#include "string"

typedef void (*OnRotaryMenuItemSelected)(int index);
typedef String (*GetMenuItemText)(int index);

class RotaryMenu {
private:

  Adafruit_seesaw* _ss;
  Adafruit_SSD1306* _display;
  int _num_items;
  int _num_encoder_positions;
  OnRotaryMenuItemSelected* _encoder_position_functions;
  GetMenuItemText* _encoder_position_names;
  bool _last_button_state;
  int _encoder_switch_i2c_pin;
  int _last_encoder_position;

  int _last_update_time = 0;  //When is the last time we drew the display
  int _refresh_time_ms = 50;  //How often to redraw the display regardless

public:
  RotaryMenu(Adafruit_seesaw* ss_param, Adafruit_SSD1306* display_param, int num_encoder_positions, int num_items, GetMenuItemText* encoder_position_names, OnRotaryMenuItemSelected* encoder_position_functions, int encoder_switch_i2c_pin = 24);
  void Update();

private:
  int GetEncoderPosition();
};