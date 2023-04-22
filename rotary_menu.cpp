#include "shared.h"
#include "rotary_menu.h"

RotaryMenu::RotaryMenu(Adafruit_seesaw *ss_param, Adafruit_SSD1306 *display_param, int num_encoder_positions, int num_items, GetMenuItemText encoder_position_names[], OnRotaryMenuItemSelected encoder_position_functions[], int encoder_switch_i2c_pin) {
  _ss = ss_param;
  _display = display_param;
  _num_items = num_items;
  _num_encoder_positions = num_encoder_positions;
  _encoder_position_functions = encoder_position_functions;
  _encoder_position_names = encoder_position_names;
  _last_button_state = false;
  _encoder_switch_i2c_pin = encoder_switch_i2c_pin;
  _last_encoder_position = -15000;
}

void RotaryMenu::Update() {
  bool need_update = false;
  int position = this->GetEncoderPosition();
  int elapsed = millis() - this->_last_update_time;
  need_update = position != _last_encoder_position || elapsed > this->_refresh_time_ms;
  _last_encoder_position = position;
  if (position >= this->_num_items)
    return;

  bool btn_pressed = !this->_ss->digitalRead(this->_encoder_switch_i2c_pin);  //Button readout is reversed
  if (btn_pressed != this->_last_button_state) {
    this->_last_button_state = btn_pressed;
    if (btn_pressed) {
      need_update = true;
      Serial.print("Pressed ");
      Serial.println(position);
      this->_encoder_position_functions[position](position);
    }
  }

  if (need_update) {
    _last_update_time = millis();
    println(this->_display, this->_encoder_position_names[position](position));
  }
}

int RotaryMenu::GetEncoderPosition() {
  int encoderPosition = _ss->getEncoderPosition() % this->_num_encoder_positions;
  if (encoderPosition < 0) {
    return encoderPosition + this->_num_encoder_positions;
  } else {
    return encoderPosition;
  }
}
