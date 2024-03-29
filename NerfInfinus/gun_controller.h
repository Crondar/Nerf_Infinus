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
  Adafruit_DCMotor *firemotor;
  Adafruit_DCMotor *guillotineMotor;
  Adafruit_DCMotor *loadingMotor;

  unsigned long _breakbeam_start_time;
  unsigned long _loadingStart_time;
  unsigned long _currentMillisPusher_time;

  int _AmmoCount;
  double _last_dart_speed_fps;
  bool _firing;
  bool _reving;
  bool _loading_in_progress;

public:

  GunController(GunConfig *config, GunState *model, Adafruit_MotorShield *afms);
  void Initialize();
  void ResetGuillotine(int gpin);
  void ResetPusher(int gpin);

  bool inline IsLoadingInProgress();

  bool IsDartInBarrel();
  bool IsReving();
  bool IsPusherReturned();
  bool IsDartReadyToFire();
  bool IsFiring();
  bool IsLoading();
  bool IsMagInserted();
  bool IsDartInLoadingPosition();
  bool IsGuillotineReturned();

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
  void OnDartInLoadingPositionChanged(bool);
  void OnGuillotineReturnChanged(bool);

  String ToString();
};