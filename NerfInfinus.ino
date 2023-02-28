/* Simple demonstration of setting and printing the current time */
/* The initial time will need to come from an RTC, NTP, or user */

/* Released to the public domain by Earle F. Philhower, III <earlephilhower@yahoo.com> */
#include <time.h>
#include <sys/time.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>
#include "Adafruit_seesaw.h"
////////////////////////////////////////////////////////
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);

// On board NeoPixel ///////////////////////////////////
#define NUMPIXELS 1
#define PIXEL_PIN 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
////////////////////////////////////////////////////////
 
////////////////////////////////////////////////////////
//Display declaration
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);
////////////////////////////////////////////////////////
 
void DisplayFPS(float FPS)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);            // Start at top-left corner
  display.print("FPS  ");
  char output[256];
  display.setTextSize(3);
  snprintf(output, 256, "%0.1f", FPS);
  display.println(output);
  display.display();
}

void DisplayAmmoCount(int ammoCount)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,-15);
  display.print("Ammo  ");
  display.setTextSize(3);
  display.println(ammoCount);
  display.display();
}

typedef void (*InputStateChangeFunction)(bool);


#define NUMPIXELS 1
#define PIXEL_PIN 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define breakbeam_pin A0
#define revPin D11
#define pusherReturnPin D12
#define magBBPin  D9
#define barrelBBPin D6
#define firePin D10
#define loadingPin D5
#define magSwitchPin D4
#define ledPin 13

//bool breakbeam_state = false;

bool reving = false;
bool firing = false;
bool jammed = false;
bool beamBroke = false;
bool hasMag = false;
bool beam_broken = ! digitalRead(breakbeam_pin);
int ammoCount = 0;

const double dart_length_ft = 2.875 / 12.0;
const long intervalPusher = 200; // Interval, in milliseconds, that the pusher should activate for in semi-auto, then wait till returned.
const long intervalLoader = 1250;// Interval, in milliseconds, that the loader activates for.
 
unsigned long breakbeam_start = 0;
 
unsigned int RED = pixels.Color(255, 0, 0);
unsigned int GREEN = pixels.Color(0, 255, 0);

const int num_LEDs = 1; 
int LED_pins[num_LEDs] = {ledPin};
bool LED_state[num_LEDs] = {};

const int num_input_pins = 8;
int inputs[num_input_pins] = { breakbeam_pin,
 revPin,
 pusherReturnPin,
 magBBPin,
 barrelBBPin, 
 firePin,
 loadingPin, 
 magSwitchPin};
bool prev_input_state[num_input_pins] = {};
bool input_state[num_input_pins] = {};
bool input_state_changed[num_input_pins] = {};

void breakbeamPinFunction(bool inputValue) {
  beam_broken = !digitalRead(breakbeam_pin); //sensorValue > 1000;
  
  if (beam_broken) {
      digitalWrite(ledPin, HIGH);
      breakbeam_start = micros();
      pixels.setPixelColor(0, RED);
      beamBroke = true;
 
      //Serial.println(beam_broken);
    }
    else {
      digitalWrite(ledPin, LOW);
      unsigned long elapsed = (micros() - breakbeam_start);
      double elapsed_sec = elapsed / 1000000.0;
      double FPS = dart_length_ft / elapsed_sec;
      char output[256];
      snprintf(output, 256, "FPS: %0.1f Elapsed: %0.1fms", FPS, elapsed_sec * 1000);
      Serial.println(output);
      pixels.setPixelColor(0, GREEN);
      DisplayFPS(FPS);
      DisplayAmmoCount(ammoCount);
    }

  pixels.show();
}

void pusherReturnPinFunction(bool inputValue) {
  bool pusherReturned = digitalRead(pusherReturnPin);
  if (pusherReturned and beamBroke) {
    ammoCount -= 1;
    beamBroke = false;
  }
  else if (pusherReturned and !beamBroke){
    ammoCount -= 1;
    jammed = true;
  }
}

void revPinFunction(bool inputValue) {
    if (digitalRead(revPin) and ammoCount >= 1 and !jammed)
  {
    reving = true;
  }
  else
  {
    reving = false;
  }
}

void firePinFunction(bool inputValue) {
    if (digitalRead(firePin) and !jammed)
  {
    firing = true;
  }
  else {
    firing = false;
  }
 
}

void magBBPinFunction(bool inputValue) {
}

void barrelBBPinFunction(bool inputValue) {
}

void loadingPinFunction(bool inputValue) {
  if (digitalRead(loadingPin)){
    ammoCount += 1;
  }
}

void magSwitchPinFunction(bool inputValue) {
  bool hasMag = digitalRead(magSwitchPin);
}

InputStateChangeFunction device_state_functions[num_input_pins] = { 
 breakbeamPinFunction, pusherReturnPinFunction,
 revPinFunction, firePinFunction,
 magBBPinFunction, barrelBBPinFunction,
 loadingPinFunction, magSwitchPinFunction};

void setup() {
  
  for(int i = 0; i < num_input_pins; i++) {
    pinMode(inputs[i], INPUT_PULLDOWN);
  }

  for(int i = 0; i < num_LEDs; i++) {
    pinMode(LED_pins[i], OUTPUT);
  }

  AFMS.begin();
  motor1->setSpeed(240);
  motor2->setSpeed(100);
 
  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(0, 150, 0));
  pixels.setBrightness(64);
  pixels.show();
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    delay(3000);
  }
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
 
  display.clearDisplay();

}

void loop() { 
  
  for (int i = 0; i < num_input_pins; i++) {
    input_state[i] = digitalRead(inputs[i]);
    input_state_changed[i] = input_state[i] != prev_input_state[i];
    prev_input_state[i] = input_state[i];
  }
 
  for (int i = 0; i < num_input_pins; i++) {
    if (input_state_changed[i]) {
      (*device_state_functions[i])(input_state[i]);
    }
  }
  
  motor1->run(reving ? FORWARD : RELEASE);
  motor2->run(firing ? FORWARD : RELEASE);
}
