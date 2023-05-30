#include "gun_controller.h"
#include "shared.h"

GunController::GunController(GunConfig *config, GunState *model, Adafruit_MotorShield *afms) {
  this->AFMS = afms;
  this->flywheelMotor = AFMS->getMotor(1);
  this->guillotineMotor = AFMS->getMotor(2);
  this->loadingMotor = AFMS->getMotor(3);
  this->config = config;
  this->model = model;
  this->_AmmoCount = config->startingAmmoCount;
  this->_last_dart_speed_fps = 0;
}

void GunController::Initialize() {
  this->flywheelMotor->setSpeed(this->config->motorPWMRange.interpolate(1.0));
  this->guillotineMotor->setSpeed(this->config->motorPWMRange.interpolate(1.0));
  this->loadingMotor->setSpeed(this->config->motorPWMRange.interpolate(1.0));
  pinMode(flywheelPWM, OUTPUT);
  pinMode(flywheelDirection, OUTPUT);
  analogWriteFreq(20000);
}

//void GunController::GunController::PusherReturned(){
//Do the thing when the pusher returns
//if(Config->FullAuto){
//}
//else {
//}
//}

bool GunController::IsDartInBarrel() {
  return this->model->IsDartInBarrel();
}
bool GunController::IsReving() {
  return this->model->IsReving();
}
bool GunController::IsPusherReturned() {
  return this->model->IsPusherReturned();
}
bool GunController::IsDartReadyToFire() {
  return this->model->IsDartReadyToFire();
}
bool GunController::IsFiring() {
  return this->model->IsFiring();
}
bool GunController::IsLoading() {
  return this->model->IsLoading();
}
bool GunController::IsMagInserted() {
  return this->model->IsMagInserted();
}
bool GunController::IsDartInLoadingPosition() {
  return this->model->IsDartInLoadingPosition();
}
bool GunController::IsGuillotineReturned() {
  return this->model->IsGuillotineReturned();
}

double GunController::GetLastDartSpeed() {
  return this->_last_dart_speed_fps;
}
int GunController::GetAmmoCount() {
  return this->_AmmoCount;
}

void GunController::SetAmmoCount(int inputValue) {
  this->_AmmoCount = inputValue;
}

//Check that the dart is likely to have fully loaded and that the guillotine will not collide with the pusher
bool inline GunController::IsLoadingInProgress() {
  unsigned long loadingTimeElapsed = (millis() - this->_loadingStart_time);
  return loadingTimeElapsed < intervalLoader;
}

void GunController::RunFlywheels(bool state, bool direction, int PWM /*PWM values 0-255 */) {
 if (state){
    analogWrite(flywheelPWM, PWM);
    digitalWrite(flywheelDirection, direction);
  }
    else {
      analogWrite(flywheelPWM, 0);
    }
}

void GunController::OnBreakbeamChanged(bool inputValue) {
  //this->IsDartInBarrel = !digitalRead(barrel_breakbeam_pin); //sensorValue > 1000;
  if (this->IsDartInBarrel()) {
    //digitalWrite(ledPin, HIGH);
    this->_breakbeam_start_time = micros();
    //pixels.setPixelColor(0, RED);
    // beamBroke = true;

    //Serial.println(This->this->IsDartInBarrel);
  } else {
    //digitalWrite(ledPin, LOW);
    unsigned long elapsed = (micros() - this->_breakbeam_start_time);
    double elapsed_sec = elapsed / 1000000.0;
    double FPS = dart_length_ft / elapsed_sec;
    char output[256];
    snprintf(output, 256, "FPS: %0.1f Elapsed: %0.1fms, TSpeed: %d", FPS, elapsed_sec * 1000, config->dartSpeed);
    Serial.println(output);
    this->_last_dart_speed_fps = FPS;
    //pixels.setPixelColor(0, GREEN);
    // DisplayFPS(FPS);
    // DisplayAmmoCount(ammoCount);
  }

  //pixels.show();
}

void GunController::OnPusherReturnChanged(bool inputValue) {
  if (config->ammoCountMode == DOWN){
    this->_AmmoCount -= inputValue;
  }
  else{
    this->_AmmoCount += inputValue;
  }
  // jammed = this->IsFiring() && breakbeam_start > micros() - 500000;
  if (config->fireMode == SEMI) {
    flywheelMotor->run(RELEASE);
  } else if (!inputValue || this->IsLoadingInProgress() || (this->IsReving() ? false : (!config->fireLockOn) ? false : true) || !model->IsDartReadyToFire() /*|| jammed */ || config->safetyOn) {
    flywheelMotor->run(RELEASE);
  }
  _firing = !inputValue || this->IsLoadingInProgress() || (this->IsReving() ? false : (!config->fireLockOn) ? false : true) || !model->IsDartReadyToFire() /*|| jammed*/ || config->safetyOn ? false : true;
  //println((inputValue) ? "pusher returned," : "pusher not returned");
  Serial.println(this->_AmmoCount);
}

void GunController::OnRevChanged(bool inputValue) {
  RunFlywheels((inputValue && (this->IsDartReadyToFire() ? true : (!config->revLockOn) ? true : false) /*&& !jammed*/&& !config->safetyOn) ? true : false, true, this->config->motorPWMRange.interpolate(0.0)); // lower bound 25
  _reving = (inputValue && (this->IsDartReadyToFire() ? true : (!config->revLockOn) ? true : false) /*&& !jammed*/ && !config->safetyOn) ? true : false;
}

void GunController::OnFireChanged(bool inputValue) {
  this->_currentMillisPusher_time = millis();
  if (inputValue) {
    flywheelMotor->run((!this->IsLoadingInProgress() && (this->model->IsReving() ? true : (!config->fireLockOn) ? true: false)&& model->IsDartReadyToFire() /*&& !jammed*/ && !config->safetyOn) ? FORWARD : RELEASE);
    _firing = (!this->IsLoadingInProgress() && (this->model->IsReving() ? true : (!config->fireLockOn) ? true : false) && model->IsDartReadyToFire() /*&& !jammed*/ && !config->safetyOn) ? true : false;
    //Serial.println(jammed);
  }
}

void GunController::OnDartReadyToFireChanged(bool inputValue) {
  //printlnWithXY((inputValue) ? "dart ready to fire" : "dart not ready to fire", 0, 15);
}

void GunController::OnDartLoadedChanged(bool inputValue) {
  this->_AmmoCount += inputValue;
  this->_loadingStart_time = millis();
  loadingMotor->run(FORWARD);
  //printlnWithXY((inputValue) ? "loading" : "not loading", 0, 15);
  Serial.println(this->IsLoadingInProgress());
}

void GunController::OnMagazineLoadedChanged(bool inputValue) {
  this->_AmmoCount = this->config->ammoCountMode == UP ? 0 : this->config->startingAmmoCount;
  //printlnWithXY((inputValue) ? "mag inserted" : "mag not inserted", 0, 15);
}

void GunController::OnDartInLoadingPositionChanged(bool inputValue) {
 if (IsPusherReturned()){
    guillotineMotor->run(FORWARD);
  }
  loadingMotor->run(RELEASE);
}

void GunController::OnGuillotineReturnChanged(bool inputValue) {
  if (inputValue) {
    guillotineMotor->run(RELEASE);
  }
}

String GunController::ToString(){
  char output[256];
  snprintf(output, 256, "DiB: %d Rev: %d, PRet: %d, DRF: %d Firing: %d Loading: %d MagInserted: %d", IsDartInBarrel(), _reving, IsPusherReturned(), IsDartReadyToFire(), _firing, IsLoading(), IsMagInserted());
  //Serial.print(output);
  String result = String(output);
  return result;
}
// ammoCountUp
// startingAmmoCountType
// chronographUnits
// safetyOn
// revLockOn
// fireLockOn
// _ammo_count_up_index
// _starting_ammo_count_type_index
// _chronograph_units_index
// _safety_on_index
// _rev_lock_on _index
// _fire_lock_on_index
