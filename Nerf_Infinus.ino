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
////////////////////////////////////////////////////////
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
 
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
 
#define NUMPIXELS 1
#define PIXEL_PIN 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
 
int loopCount = 0;
 
int breakbeam_pin = A0;
int ledPin = 13;
bool breakbeam_state = false;
 
int revPin = D11;
int pusherReturnPin = D12;
int magBBPin = D9;
int barrelBBPin = D6;
int firePin = D10;
int loadingPin = D5;
int magSwitchPin = D4;
 
bool reving = false;
bool jammed = false;
bool beamBroke = false;
bool hasMag = false;
int ammoCount = 0;

 
const float dart_length_ft = 2.875 / 12.0;
 
unsigned long breakbeam_start = 0;
 
unsigned int RED = pixels.Color(255, 0, 0);
unsigned int GREEN = pixels.Color(0, 255, 0);
 
 
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
 
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(A0, INPUT_PULLUP);
 
  pinMode(revPin, INPUT);
  pinMode(pusherReturnPin, INPUT);
  pinMode(magBBPin, INPUT);
  pinMode(barrelBBPin, INPUT);
  pinMode(firePin, INPUT);
  pinMode(loadingPin, INPUT);
  pinMode(magSwitchPin, INPUT);
 
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
    delay(5000);
  }
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
 
  display.clearDisplay();
}
 
void loop() {
  int sensorValue = digitalRead(breakbeam_pin);
  bool beam_broken = !sensorValue; //sensorValue > 1000;
  bool pusher_returned = digitalRead(pusherReturnPin);
  
  //Serial.println(sensorValue);
  if (beam_broken != breakbeam_state)
  {
    breakbeam_state = beam_broken;
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
      float elapsed_sec = elapsed / 1000000.0;
      float FPS = dart_length_ft / elapsed_sec;
      char output[256];
      snprintf(output, 256, "FPS: %0.1f Elapsed: %0.1fms", FPS, elapsed_sec * 1000);
      Serial.println(output);
      pixels.setPixelColor(0, GREEN);
      DisplayFPS(FPS);
      DisplayAmmoCount(ammoCount);
    }
 
    pixels.show();
  }
  
  if (pusher_returned and beamBroke)
  {
    beamBroke = false;
    ammoCount -= 1;
  }
  else if (pusher_returned)
  {
    jammed = true;
  }
  else
  {
    jammed = true;
  }
 
  if (digitalRead(revPin) and ammoCount >= 1 and !jammed)
  {
    reving = true;
  }
  else
  {
    reving = false;
  }
 
  motor1->run(reving ? FORWARD : RELEASE);
  /*
  if (reving)
  {
    motor1->run(FORWARD);
  }
  else {
    motor1->run(RELEASE);
  }*/
 
  if (digitalRead(firePin) and !jammed)
  {
    motor2->run(FORWARD);
  }
  else {
    motor2->run(RELEASE);
  }
 
  if (digitalRead(loadingPin))
  {
    ammoCount += 1;
    Serial.print(ammoCount, DEC);
  }
 
  hasMag = digitalRead(magSwitchPin);
 
  if (!hasMag)
  {
    ammoCount = 0;
  }
}
 
 
