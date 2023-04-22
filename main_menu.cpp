#include "main_menu.h"

const int num_menu_items = 13;

GunController *gun;
GunConfig *config;

RotaryMenu *MainMenu;

GetMenuItemText *main_menu_item_names;

String GetFireModeStr(int index) {
  const static String semi_str = String("Fire Mode: SEMI");
  const static String auto_str = String("Fire Mode: AUTO");
  return config->fireMode == SEMI ? semi_str : auto_str;
}

void setFireMode(int index) {
  config->fireMode = config->fireMode == SEMI ? AUTO : SEMI;
  //main_menu_item_names[index] = GetFireModeStr();
}

String GetFireRateStr(int index) {
  char output[64];
  snprintf(output, 64, "Fire Rate: %02d", config->fireRate);
  Serial.print(output);
  return String(output);
}

void setFireRate(int index) {
  if (config->fireRate < 10) {
    config->fireRate += 1;
  } else {
    config->fireRate = 1;
  }
}

String GetDartSpeedStr(int index) {
  char output[64];
  snprintf(output, 64, "Dart Speed: %02d", config->dartSpeed);
  Serial.print(output);
  return String(output);
}

void AddDartSpeed(int index) {
  //TODO: Launch a submenu
  if (config->dartSpeed < 170) {
    config->dartSpeed += 10;
  } else {
    config->dartSpeed = 70;
  }
}

String GetMagAmmoCountStr(int index) {
  char output[64];
  snprintf(output, 64, "Mag Ammo: %02d", config->startingAmmoCount);
  Serial.print(output);
  return String(output);
}

void SetMagAmmoCount(int index) {
  //Todo: Use submenu
  /*
  if (lastPressed == 6) {
      if (startingAmmoCount < 35){
    startingAmmoCount += 1;
  }
  else{
    startingAmmoCount = 35;
    lastPressed = 7;
  }
  }
  else{
    if (ammoCount < 35){
    ammoCount += 1;
  }
  else{
    ammoCount = 35;
  }
  lastPressed = 7;
  }*/
}

String GetCurrentAmmoCountStr(int index) {
  const static String ammo_str = String("Ammo:");

  char output[32];
  snprintf(output, 32, " %02d", gun->GetAmmoCount());
  Serial.print("Menu ammo: ");
  Serial.println(gun->GetAmmoCount());

  return ammo_str + output;
}

void SetCurrentAmmoCount(int index) {
  //Todo: Use submenu
  //main_menu_item_names[index] = GetCurrentAmmoCountStr();
  /*
  if (lastPressed == 6) {
      if (startingAmmoCount < 35){
    startingAmmoCount += 1;
  }
  else{
    startingAmmoCount = 35;
    lastPressed = 7;
  }
  }
  else{
    if (ammoCount < 35){
    ammoCount += 1;
  }
  else{
    ammoCount = 35;
  }
  lastPressed = 7;
  }*/
}

String GetAmmoCountDirectionStr(int index) {
  const static String up_str = String("Ammo Counts: Up");
  const static String down_str = String("Ammo Counts: Down");
  return config->ammoCountMode == UP ? up_str : down_str;
}

void SetAmmoCountDirection(int index) {
  config->ammoCountMode = config->ammoCountMode == UP ? DOWN : UP;
  gun->SetAmmoCount(config->ammoCountMode == UP ? 0 : 35);
  //main_menu_item_names[index] = GetAmmoCountDirectionStr();
}

String GetChronographUnitsStr(int index) {
  const static String fps_str = String("Speed Units:\n  Feet / sec");
  const static String mps_str = String("Speed Units:\n  Meters / sec");

  char output[32];
  snprintf(output, 32, "\nLast dart speed: %0.1f", gun->GetLastDartSpeed());
  Serial.println(output);

  return config->chronographUnits == FPS ? fps_str + output : mps_str + output;
}

void SetChronographUnits(int index) {
  config->chronographUnits = config->chronographUnits == FPS ? MPS : FPS;
  //main_menu_item_names[index] = GetChronographUnitsStr();
}

String GetSafetyStr(int index) {
  const static String on_str = String("Safety: On");
  const static String off_str = String("Safety: Off");
  return config->safetyOn ? on_str : off_str;
}

void SetSafety(int index) {
  config->safetyOn = !config->safetyOn;
  //main_menu_item_names[index] = GetSafetyStr();
}

String GetRevLockStr(int index) {
  const static String on_str = String("Rev Lock: On");
  const static String off_str = String("Rev Lock: Off");
  return config->revLockOn ? on_str : off_str;
}

void SetRevLock(int index) {
  config->revLockOn = !config->revLockOn;
  //main_menu_item_names[index] = GetRevLockStr();
}

String GetFireLockStr(int index) {
  const static String on_str = String("Fire Lock: On");
  const static String off_str = String("Fire Lock: Off");
  return config->fireLockOn ? on_str : off_str;
}

void SetFireLock(int index) {
  config->fireLockOn = !config->fireLockOn;
  //main_menu_item_names[index] = GetFireLockStr();
}

void SetUpdatePresetSettings(int index) {
}

void SetDefaultSettings(int index) {
  //changeMe
}

void SetDisplayUserManual(int index) {
  //changeMe
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//temp str
String GetFiringStr(int index) {
  char output[64];
  snprintf(output, 64, "Firing: %02d", gun->IsFiring());
  Serial.print(output);
  return String(output);  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

OnRotaryMenuItemSelected main_menu_item_clicked_functions[num_menu_items] = {
  setFireMode,
  setFireRate,
  AddDartSpeed,
  SetMagAmmoCount,
  SetCurrentAmmoCount,
  SetAmmoCountDirection,
  SetChronographUnits,
  SetSafety,
  SetRevLock,
  SetFireLock,
  SetUpdatePresetSettings,
  SetDefaultSettings,
  //presets go here
  SetDisplayUserManual
};

String GetNotImplementedString(int index) {
  return String("Not Implemented");
}


//Specify the gun config that is being updated
void MainMenuInitialize(Adafruit_seesaw *ss_param, Adafruit_SSD1306 *display_param, int num_encoder_positions, GunController *g) {
  gun = g;
  config = gun->GetGunConfig();

  /*char buffer[64];
  snprintf(buffer, 64, "Main Menu Gun Controller = %p\n", gun);
  Serial.println(buffer);*/

  main_menu_item_names = new GetMenuItemText[num_menu_items]{
    GetFireModeStr,
    GetFireRateStr,           //String("setFireRate"),
    GetDartSpeedStr,  //String("AddDartSpeed"),
    GetMagAmmoCountStr,  //String("SetMagAmmoCount"),
    GetCurrentAmmoCountStr,
    GetAmmoCountDirectionStr,
    GetChronographUnitsStr,
    GetSafetyStr,
    GetRevLockStr,
    GetFireLockStr,
    GetFiringStr,  //String("SetUpdatePresetSettings"),
    GetNotImplementedString,  //String("SetDefaultSettings"),
    //presets go here
    GetNotImplementedString,  //String("SetDisplayUserManual")
  };

  MainMenu = new RotaryMenu(ss_param, display_param, num_encoder_positions, num_menu_items, main_menu_item_names, main_menu_item_clicked_functions);
}