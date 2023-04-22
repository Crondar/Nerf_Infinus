#pragma once

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

  // int barrel_breakbeam_index;
  // int reving_index;
  // int pusher_return_index;
  // int dart_ready_to_fire_index;
  // int fire_index;
  // int is_loading_index;
  // int magazine_inserted_index;
  // int encoder_button_pressed_index;
  // int fire_mode_index;
  // int ammo_count_up_index;
  // int starting_ammo_count_type_index;
  // int chronograph_units_index;
  // int safety_on_index;
  // int rev_lock_on_index;
  // int fire_lock_on_index;

  GunConfig(int fireRate, int dartSpeed, int startingAmmoCount, FireMode fireMode, AmmoCountMode ammoCountMode, ChronographUnits chronographUnits, bool safetyOn, bool revLockOn, bool fireLockOn);
  GunConfig();
  // GunConfig();

  // bool inline IsDartInBarrel();
  // bool inline IsReving();
  // bool inline IsPusherReturned();
  // bool inline IsDartReadyToFire();
  // bool inline IsFiring();
  // bool inline IsLoading();
  // bool inline IsMagInserted();
  // bool inline IsEncoderButtonPressed();
  // bool inline IsfireModeSemi();
  // bool inline IsAmmoCountUp();
  // bool inline IsStartingAmmoCount();
  // bool inline IsChronographUnits();
  // bool inline IsSafetyOn();
  // bool inline IsRevLockOn ();
  // bool inline IsFireLockOn();
};