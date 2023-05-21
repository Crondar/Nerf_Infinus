#pragma once
#include "range.h"

enum FireMode { SEMI,
                AUTO };
enum AmmoCountMode { UP,
                     DOWN };
enum ChronographUnits { FPS,
                        MPS };  //Feet Per Second (FPS) or Meters Per Second (MPS)

struct GunConfig {
public:
  int fireRate;                 //Target fire rate for gun
  int dartSpeed;                //Target dart speed for gun
  int startingAmmoCount;        //How much ammo in a fresh mag
  FireMode fireMode;            //Semi or Auto fire mode
  AmmoCountMode ammoCountMode;  //Direction to count ammo, defaults to DOWN
  ChronographUnits chronographUnits;
  bool safetyOn;    //Is the gun allowed to fire or rev?
  bool revLockOn;   //Is the gun allowed to rev when there is no dart in the chamber
  bool fireLockOn;  //Is the gun allowed to fire when the gun is not rev'ing (for testing)
  Range motorPWMRange;



  GunConfig(int fireRate, int dartSpeed, int startingAmmoCount, FireMode fireMode, AmmoCountMode ammoCountMode, ChronographUnits chronographUnits, bool safetyOn, bool revLockOn, bool fireLockOn, Range motorPWMRange);
  GunConfig();

  };