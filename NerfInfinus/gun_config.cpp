#include "gun_config.h"

GunConfig::GunConfig() {
  this->motorPWMRange = Range(25, 255);
  this->fireRate = 2;
  this->dartSpeed = 130;
  this->startingAmmoCount = 35;
  this->fireMode = SEMI;
  this->ammoCountMode = DOWN;
  this->chronographUnits = FPS;
  this->safetyOn = false;
  this->revLockOn = true;
  this->fireLockOn = true;
}

GunConfig::GunConfig(int fireRate, int dartSpeed, int startingAmmoCount, FireMode fireMode, AmmoCountMode ammoCountMode, ChronographUnits chronographUnits, bool safetyOn, bool revLockOn, bool fireLockOn, Range motorPWMRange) {
  this->fireRate = fireRate;
  this->dartSpeed = dartSpeed;
  this->startingAmmoCount = startingAmmoCount;
  this->fireMode = fireMode;
  this->ammoCountMode = ammoCountMode;
  this->chronographUnits = chronographUnits;
  this->safetyOn = safetyOn;
  this->revLockOn = revLockOn;
  this->fireLockOn = fireLockOn;
  this->motorPWMRange = Range(motorPWMRange.minVal(), motorPWMRange.maxVal());
}
