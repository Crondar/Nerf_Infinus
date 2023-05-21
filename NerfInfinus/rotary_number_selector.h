#include "shared.h"
#include "Adafruit_seesaw.h"
#include <Adafruit_SSD1306.h>
#include "string"

typedef void (*OnRotaryNumberSelected)(void *selector, int number);


class RotaryNumberSelector {
private:

  Adafruit_seesaw *_ss;
  Adafruit_SSD1306 *_display;
  OnRotaryNumberSelected _on_number_selected;
  bool _last_button_state;
  int _encoder_switch_i2c_pin;
  int _num_encoder_positions;
  int _last_encoder_position;
  int _min_val;
  int _max_val;

public:
  RotaryNumberSelector(Adafruit_seesaw *ss_param, Adafruit_SSD1306 *display_param, int min_val, int max_val, int start_val, OnRotaryNumberSelected encoder_position_functions, int encoder_switch_i2c_pin = 24);
  void Update();

  int Value;  //The number the user has dialed in

private:
  int GetEncoderPosition();
};
