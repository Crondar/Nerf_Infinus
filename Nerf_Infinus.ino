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
#define WIRE Wire  
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
 
#define barrel_breakbeam_pin 26 //A0
#define revPin 11 // D11
#define pusherReturnPin 12 // D12
#define dart_ready_to_fire_pin 9 // D9
#define barrelBBPin 8 // D6
#define firePin 10 //D10
#define loadingPin 7 //D5
#define magSwitchPin 6 //D4
#define ledPin 16  // 13

#define ENCODER_SWITCH_I2C_PIN 24 //default

#define SEESAW_ADDR          0x36

//bool breakbeam_state = false;

int ammoCount = 0;

//Set jammed to true to stop the motors.  Reset by rebooting the program.
bool jammed = false; 
const double dart_length_ft = 2.875 / 12.0;
const long intervalPusher = 200; // Interval, in milliseconds, that the pusher should activate for in semi-auto, then wait till returned.
const long intervalLoader = 1250;// Interval, in milliseconds, that the loader activates for.
 
unsigned long breakbeam_start = 0;
 
unsigned int RED = pixels.Color(255, 0, 0);
unsigned int GREEN = pixels.Color(0, 255, 0);

const int num_LEDs = 0; 
int LED_pins[num_LEDs] = {};
bool LED_state[num_LEDs] = {};

////////////////////////////////////////////////////////////////////
// Arrays to store pin state at start of each loop
const int num_input_pins = 7;
int inputs[num_input_pins] = { barrel_breakbeam_pin,
 revPin,
 pusherReturnPin,
 dart_ready_to_fire_pin, 
 firePin,
 loadingPin, 
 magSwitchPin};


const int num_inputs = num_input_pins + 1; //+1 for rotary encoder
bool prev_input_state[num_inputs] = {};
bool input_state[num_inputs] = {};
bool input_state_changed[num_inputs] = {};

size_t println(const String &s)
{
  size_t result = Serial.println(s);
  display.clearDisplay();
  display.setCursor(0, 0);  
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.println(s);
  display.display();
  return result;
}

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

//Begin I2C controls
#define ENCODER_BUTTON_PRESSED_INDEX 7

Adafruit_seesaw ss = Adafruit_seesaw(&Wire1);

bool inline IsDartInBarrel() {return !input_state[BARREL_BREAKBEAM_INDEX]; }
bool inline IsReving() { return input_state[REVING_INDEX]; }
bool inline IsPusherReturned() { return input_state[PUSHER_RETURN_INDEX]; }
bool inline IsDartReadyToFire() { return input_state[DART_READY_TO_FIRE_INDEX];} 
bool inline IsFiring() { return input_state[FIRE_INDEX]; }
bool inline IsLoading() {return input_state[IS_LOADING_INDEX];}
bool inline IsMagInserted() {return input_state[MAGAZINE_INSERTED_INDEX];}
bool inline IsEncoderButtonPressed() {return input_state[ENCODER_BUTTON_PRESSED_INDEX];}
////////////////////////////////////////////////////////////////////

bool beamBroke = false;

void breakbeamPinFunction(bool inputValue) {
  //IsDartInBarrel = !digitalRead(barrel_breakbeam_pin); //sensorValue > 1000;
  
  if (IsDartInBarrel()) {
      digitalWrite(ledPin, HIGH);
      breakbeam_start = micros();
      pixels.setPixelColor(0, RED);
      beamBroke = true;
 
      //Serial.println(IsDartInBarrel);
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
  motor1->run(IsReving ? FORWARD : RELEASE);
}

void firePinFunction(bool inputValue) {
  motor2->run(IsFiring ? FORWARD : RELEASE);
}

void dart_ready_to_fire_pinFunction(bool inputValue) {
}

void loadingPinFunction(bool inputValue) {
  if (digitalRead(loadingPin)){
    ammoCount += 1;
  }
}

void magSwitchPinFunction(bool inputValue) {
  bool hasMag = digitalRead(magSwitchPin);
}

void encoderButtonPressedFunction(bool inputValue){
  bool is_pressed = !inputValue; //Rotary button is reversed output
  if(is_pressed)
    println("Pressed");
  else
    println("Release");
}


InputStateChangeFunction device_state_functions[num_inputs] = { 
  breakbeamPinFunction, 
  pusherReturnPinFunction,
  revPinFunction, 
  firePinFunction,
  dart_ready_to_fire_pinFunction,
  loadingPinFunction,
  magSwitchPinFunction, 
  encoderButtonPressedFunction
 };

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  println("hello world");

  println("Init Pixels");
  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(0, 128, 0));
  pixels.setBrightness(64);
  pixels.show();

  for(int i = 0; i < num_input_pins; i++) {
    pinMode(inputs[i], INPUT_PULLDOWN);
  }

  for(int i = 0; i < num_LEDs; i++) {
    pinMode(LED_pins[i], OUTPUT);
  }

  Serial.println("Init Display");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    delay(3000);
  }
else
{
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
 
  display.clearDisplay();
  println("Display cleared"); 
}
  println("Init AFMS");
  if (!AFMS.begin(1600, &Wire1)) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    println("Could not find Motor Shield. Check wiring.");
    
    while (1);
  }
  else{ 
    motor1->setSpeed(240);
    motor2->setSpeed(100);
  }
   
  if (! ss.begin(SEESAW_ADDR) ) {
  Serial.println("Couldn't find seesaw on default address");
  while(1) delay(10);
  }
  Serial.println("seesaw started");

 
  ss.setGPIOInterrupts((uint32_t)1 << ENCODER_SWITCH_I2C_PIN, 1);
  println("Init GPIO Interrupts");
  ss.enableEncoderInterrupt();
  println("Seesaw Encoder Interrupts enabled");

  ss.pinMode(ENCODER_SWITCH_I2C_PIN, INPUT_PULLUP);
}

void loop() {  

  for (int i = 0; i < num_input_pins; i++) {
    input_state[i] = digitalRead(inputs[i]);
  }

  input_state[num_input_pins] = ss.digitalRead(ENCODER_SWITCH_I2C_PIN);

  for(int i = 0; i < num_inputs; i++){
    input_state_changed[i] = input_state[i] != prev_input_state[i];
    prev_input_state[i] = input_state[i];
  }
 
  for (int i = 0; i < num_inputs; i++) {
    if (input_state_changed[i]) {
      (*device_state_functions[i])(input_state[i]);
    }
  }
}
