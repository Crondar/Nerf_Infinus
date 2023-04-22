#include "rotary_number_selector.h"

RotaryNumberSelector::RotaryNumberSelector(Adafruit_seesaw *ss_param, Adafruit_SSD1306 *display_param, int min_val, int max_val, int start_val, OnRotaryNumberSelected on_number_selected, int encoder_switch_i2c_pin) {
  _ss = ss_param;
  _display = display_param;
  Value = start_val;
  _min_val = min_val;
  _max_val = max_val;
  _on_number_selected = on_number_selected;
  _encoder_switch_i2c_pin = encoder_switch_i2c_pin;
}

void RotaryNumberSelector::Update() {

  bool btn_pressed = !this->_ss->digitalRead(this->_encoder_switch_i2c_pin);  //Button readout is reversed
  if (btn_pressed != this->_last_button_state) {
    if (btn_pressed) {
      Serial.print("Selected ");
      Serial.println(this->Value);
      this->_on_number_selected(this, this->Value);  //Ideally the caller stops us here
    }
  }

  int position = this->GetEncoderPosition();
  int direction = position - this->_last_encoder_position;
  if (direction == 0)
    return;

  //Clip value to desired range
  Value += direction;
  Value = Value < _min_val ? _min_val : Value;
  Value = Value > _max_val ? _max_val : Value;

  char output[8];
  snprintf(output, 8, "%03d", Value);

  println(this->_display, 2, output);
}

int RotaryNumberSelector::GetEncoderPosition() {
  return _ss->getEncoderPosition();
}