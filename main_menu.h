#pragma once

#include <Adafruit_SSD1306.h>
#include "Adafruit_seesaw.h"
#include "gun_config.h"
#include "gun_controller.h"
#include "rotary_menu.h"

extern RotaryMenu *MainMenu;
void MainMenuInitialize(Adafruit_seesaw *ss_param, Adafruit_SSD1306 *display_param, int num_encoder_positions, GunController *gun);