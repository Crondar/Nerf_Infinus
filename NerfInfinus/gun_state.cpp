#include "gun_state.h"

const int _barrel_breakbeam_index = 0;
const int _reving_index = 1;
const int _pusher_return_index = 2;
const int _dart_ready_to_fire_index = 3;
const int _fire_index = 4;
const int _is_loading_index = 5;
const int _magazine_inserted_index = 6;
const int _dart_in_loading_position_index = 7;
const int _guillotine_return_index = 8;

const int number_of_sensors = 9;

GunState::GunState(bool *input_state, bool *prev_input_state, bool *input_state_changed) {
  this->prev_input_state = prev_input_state;
  this->input_state = input_state;
  this->input_state_changed = input_state_changed;
  this->num_input_pins = number_of_sensors;
}

GunState::GunState() {
  this->num_input_pins = number_of_sensors;
  input_state = new bool[number_of_sensors];          //Array of input states
  prev_input_state = new bool[number_of_sensors];     //Array of previous input states
  input_state_changed = new bool[number_of_sensors];  //Array of input states that have changed from previous value
}

bool GunState::IsDartInBarrel() {
  return !this->input_state[_barrel_breakbeam_index];
}
bool GunState::IsReving() {
  return this->input_state[_reving_index];
}
bool GunState::IsPusherReturned() {
  return this->input_state[_pusher_return_index];
}
bool GunState::IsDartReadyToFire() {
  return !this->input_state[_dart_ready_to_fire_index];
}
bool GunState::IsFiring() {
  return this->input_state[_fire_index];
}
bool GunState::IsLoading() {
  return this->input_state[_is_loading_index];
}
bool GunState::IsMagInserted() {
  return this->input_state[_magazine_inserted_index];
}
bool GunState::IsDartInLoadingPosition(){
  return this->input_state[_dart_in_loading_position_index];
}
bool GunState::IsGuillotineReturned(){
  return this->input_state[_guillotine_return_index];
}

// String GunState::ToString(){
//   char output[256];
//   snprintf(output, 256, "DiB: %d Rev: %d, PRet: %d, DRF: %d Firing: %d Loading: %d MagInserted: %d", IsDartInBarrel(), IsReving(), IsPusherReturned(), IsDartReadyToFire(), IsFiring(), IsLoading(), IsMagInserted());
//   //Serial.print(output);
//   String result = String(output);
//   return result;
// }