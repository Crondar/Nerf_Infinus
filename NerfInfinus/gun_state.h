#pragma once

#include <Arduino.h>

struct GunState {
public:
  bool *input_state;          //Array of input states
  bool *prev_input_state;     //Array of previous input states
  bool *input_state_changed;  //Array of input states that have changed from previous value
  int num_input_pins;

  GunState(bool *input_state, bool *prev_input_state, bool *input_state_changed);
  GunState();

  bool IsDartInBarrel();
  bool IsReving();
  bool IsPusherReturned();
  bool IsDartReadyToFire();
  bool IsFiring();
  bool IsLoading();
  bool IsMagInserted();

  // String ToString();
};