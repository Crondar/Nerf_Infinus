#pragma once

#include <time.h>
#include <sys/time.h>
#include <Adafruit_MotorShield.h>
#include "constants.h"
#include "gun_state.h"
#include "gun_config.h"
#include "range.h"


class GunController {
private:
  GunConfig *config;
  GunState *model;

  Adafruit_MotorShield *AFMS;
  Adafruit_DCMotor *motor1;

  unsigned long _breakbeam_start_time;
  unsigned long _loadingStart_time;
  unsigned long _currentMillisPusher_time;

  int _AmmoCount;
  double _last_dart_speed_fps;
  bool _firing;
  bool _reving;

public:

  GunController(GunConfig *config, GunState *model, Adafruit_MotorShield *afms);
  void Initialize();

  bool inline IsLoadingInProgress();

  bool IsDartInBarrel();
  bool IsReving();
  bool IsPusherReturned();
  bool IsDartReadyToFire();
  bool IsFiring();
  bool IsLoading();
  bool IsMagInserted();

  void SetAmmoCount(int);

  void RunFlywheels(bool, bool, int);

  double GetLastDartSpeed();
  int GetAmmoCount();  //Current ammo count the gun believes it has
  inline GunConfig *GetGunConfig() {
    return config;
  }

  void OnBreakbeamChanged(bool);
  void OnRevChanged(bool);
  void OnPusherReturnChanged(bool);
  void OnDartReadyToFireChanged(bool);
  void OnFireChanged(bool);
  void OnDartLoadedChanged(bool);
  void OnMagazineLoadedChanged(bool);

  String ToString();
};