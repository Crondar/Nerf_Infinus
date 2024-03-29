#include <arduino.h>
#include <time.h>
#include <sys/time.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_seesaw.h"
////////////////////////////////////////////////////////
#include <Adafruit_MotorShield.h>
#include "shared.h"

#include "rotary_menu.h"
#include "gun_config.h"
#include "gun_state.h"
#include "gun_controller.h"
#include "constants.h"

#include "main_menu.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// On board NeoPixel ///////////////////////////////////
#define NUMPIXELS 1
#define PIXEL_PIN 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
//Display declaration
#define WIRE Wire
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
////////////////////////////////////////////////////////

void DisplayFPS(float FPS) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);  // Start at top-left corner
  display.print("FPS  ");
  char output[256];
  display.setTextSize(3);
  snprintf(output, 256, "%0.1f", FPS);
  display.println(output);
  display.display();
}

void DisplayAmmoCount(int ammoCount) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, -15);
  display.print("Ammo  ");
  display.setTextSize(3);
  display.println(ammoCount);
  display.display();
}

typedef void (*InputStateChangeFunction)(bool);


//needs to be D_, A_, etc. to work
#define barrel_breakbeam_pin A2
#define revPin D11
#define pusherReturnPin D12
#define dart_ready_to_fire_pin D9
// #define barrelBBPin D6
#define firePin D10
#define loadingPin 7
#define magSwitchPin 6
#define dartStopSwitchPin D13
#define guillotineReturnPin 8
#define ledPin 16  // 13



#define ENCODER_SWITCH_I2C_PIN 24  //default

#define SEESAW_ADDR 0x36


//Set jammed to true to stop the motors.  Reset by rebooting the program.
bool jammed = false;

unsigned int RED = pixels.Color(255, 0, 0);
unsigned int GREEN = pixels.Color(0, 255, 0);

int LED_pins[num_LEDs] = {};
bool LED_state[num_LEDs] = {};

////////////////////////////////////////////////////////////////////
// Arrays to store pin state at start of each loop
uint8_t inputs[num_input_pins] = { barrel_breakbeam_pin,
revPin,
pusherReturnPin,
dart_ready_to_fire_pin,
firePin,
loadingPin,
magSwitchPin,
dartStopSwitchPin,
guillotineReturnPin };


const int num_inputs = num_input_pins;
bool prev_input_state[num_inputs] = {};
bool input_state[num_inputs] = {};
bool input_state_changed[num_inputs] = {};



////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Helper functions to determine state without reading IO each time
// Begin DigitalIO pins
#define BARREL_BREAKBEAM_INDEX 0
#define REVING_INDEX 1
#define PUSHER_RETURN_INDEX 2
#define DART_READY_TO_FIRE_INDEX 3
#define FIRE_INDEX 4
#define IS_LOADING_INDEX 5
#define MAGAZINE_INSERTED_INDEX 6
#define DART_IN_LOADING_POSITION_INDEX 7
#define GUILLOTINE_RETURN_INDEX 8 

//Begin I2C controls
#define ENCODER_BUTTON_PRESSED_INDEX 9

Adafruit_seesaw ss = Adafruit_seesaw(&Wire);

bool inline IsDartInBarrel() {
  return !input_state[BARREL_BREAKBEAM_INDEX];
}
bool inline IsReving() {
  return input_state[REVING_INDEX];
}
bool inline IsPusherReturned() {
  return input_state[PUSHER_RETURN_INDEX];
}
bool inline IsDartReadyToFire() {
  return input_state[DART_READY_TO_FIRE_INDEX];
}
bool inline IsFiring() {
  return input_state[FIRE_INDEX];
}
bool inline IsLoading() {
  return input_state[IS_LOADING_INDEX];
}
bool inline IsMagInserted() {
  return input_state[MAGAZINE_INSERTED_INDEX];
}
bool inline IsEncoderButtonPressed() {
  return input_state[ENCODER_BUTTON_PRESSED_INDEX];
}

// bool inline IsLoadingInProgress() {unsigned long loadingTimeElapsed = (millis() - loadingStart); //needs to be millis()
// return  loadingTimeElapsed < intervalLoader;}
////////////////////////////////////////////////////////////////////
//start of UI functions and variables


//put future presets here.

void SetDisplayUserManual() {
  //changeMe
}


//RotaryMenu rotary_menu(&ss, &display, num_encoder_positions, num_menu_items, menu_item_names, encoder_position_functions);

GunConfig gun_config = GunConfig();

GunState gun_state = GunState();

GunController gun_controller(&gun_config, &gun_state, &AFMS);

////////////////////////////////////////////////////////////////////
//OS functions
void OnBreakbeamChanged(bool inputValue) {
  gun_controller.OnBreakbeamChanged(inputValue);
}

void OnRevChanged(bool inputVal) {
  gun_controller.OnRevChanged(inputVal);
}

void OnPusherReturnChanged(bool inputVal) {
  gun_controller.OnPusherReturnChanged(inputVal);
}

void OnDartReadyToFireChanged(bool inputVal) {
  gun_controller.OnDartReadyToFireChanged(inputVal);
}

void OnFireChanged(bool inputVal) {
  gun_controller.OnFireChanged(inputVal);
}

void OnDartLoadedChanged(bool inputVal) {
  gun_controller.OnDartLoadedChanged(inputVal);
}

void OnMagazineLoadedChanged(bool inputVal) {
  gun_controller.OnMagazineLoadedChanged(inputVal);
}

void OnDartInLoadingPositionChanged(bool inputVal) {
  gun_controller.OnDartInLoadingPositionChanged(inputVal);
}

void OnGuillotineReturnChanged(bool inputVal) {
  gun_controller.OnGuillotineReturnChanged(inputVal);
}

InputStateChangeFunction device_state_functions[num_inputs] = {
  OnBreakbeamChanged,
  OnRevChanged,
  OnPusherReturnChanged,
  OnDartReadyToFireChanged,
  OnFireChanged,
  OnDartLoadedChanged,
  OnMagazineLoadedChanged,
  OnDartInLoadingPositionChanged,
  OnGuillotineReturnChanged
};

//////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  //display does not work without serial monitor

  // println(&display, "hello world");
  println(&display, "Init Pixels");
  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(0, 128, 0));
  pixels.setBrightness(64);
  pixels.show();

  for (int i = 0; i < num_input_pins; i++) {
    pinMode(inputs[i], INPUT_PULLDOWN);
  }

  for (int i = 0; i < num_LEDs; i++) {
    pinMode(LED_pins[i], OUTPUT);
  }

  Serial.println("Init Display");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    delay(3000);
  } else {
    display.cp437(true);  // Use full 256 char 'Code Page 437' font

    display.clearDisplay();
    println(&display, "Display cleared");
  }

  println(&display, "Display Works!");

  while(!ss.begin(SEESAW_ADDR)) {
    Serial.println("Couldn't find seesaw on default address");
    println(&display, "No Seesaw!");
    delay(2000);
  }

  Serial.println("seesaw started");

  println(&display, "Init AFMS");
  if (!AFMS.begin(/*1600, &Wire1*/)) {  // create with the default frequency 1.6KHz
                                    // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
  }

  Serial.println("AFMS Started");

  ss.setGPIOInterrupts((uint32_t)1 << ENCODER_SWITCH_I2C_PIN, 1);
  println(&display, "Init GPIO Interrupts");
  ss.enableEncoderInterrupt();
  println(&display, "Seesaw Encoder Interrupts enabled");

  ss.pinMode(ENCODER_SWITCH_I2C_PIN, INPUT_PULLUP);

  pinMode(barrel_breakbeam_pin, INPUT_PULLUP);  //breakbeam pin needs to be input pullup to be read

  println(&display, "Breakbeam pin configured");
  //Everything is in place, initialize the gun
  gun_controller.Initialize();

  println(&display, "Gun Controller initialized");

  /*char buffer[64];
  snprintf(buffer, 64, "Gun Controller = %p\n", &gun_controller);
  Serial.println(buffer);*/

  MainMenuInitialize(&ss, &display, num_encoder_positions, &gun_controller);

  println(&display, "Main menu initialized");
  //Create rotary menu
  //pass it rotary encoder object (ss?)
  //pass the display object or function to print output to
  //
  delay(10);
  update_gun_state();

  gun_controller.ResetPusher(inputs[PUSHER_RETURN_INDEX]);
  gun_controller.ResetGuillotine(inputs[GUILLOTINE_RETURN_INDEX]);
}

void update_gun_state()
{
 for (int i = 0; i < gun_state.num_input_pins; i++) {
    gun_state.input_state[i] = digitalRead(inputs[i]);
    gun_state.input_state_changed[i] = gun_state.input_state[i] != gun_state.prev_input_state[i];
    gun_state.prev_input_state[i] = gun_state.input_state[i];
  }
}

void loop() {

  update_gun_state();

  bool any_changed = false;

  for (int i = 0; i < num_inputs; i++) {
    if (gun_state.input_state_changed[i]) {
      (*device_state_functions[i])(gun_state.input_state[i]);
      any_changed = true;
    }
  }
 
  if(any_changed)
  {
    Serial.println(gun_controller.ToString());
  }

  MainMenu->Update(); 

}