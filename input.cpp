#include <Arduino.h>
#include "input.h"
#include "constants.h"

#ifdef SNES_CONTROLLER
uint16_t buttons = 0;

void input_setup() {
  // Set DATA_CLOCK normally HIGH
  pinMode(DATA_CLOCK, OUTPUT);
  digitalWrite(DATA_CLOCK, HIGH);

  // Set DATA_LATCH normally LOW
  pinMode(DATA_LATCH, OUTPUT);
  digitalWrite(DATA_LATCH, LOW);

  // Set DATA_SERIAL normally HIGH
  pinMode(DATA_SERIAL, OUTPUT);
  digitalWrite(DATA_SERIAL, HIGH);
  pinMode(DATA_SERIAL, INPUT);
}

void getControllerData(void){
  digitalWrite(DATA_LATCH, HIGH);
  delayMicroseconds(12);
  digitalWrite(DATA_LATCH, LOW);
  delayMicroseconds(6);
  buttons = 0;
  for(uint8_t i = 0; i < 16; ++i){
    digitalWrite(DATA_CLOCK, LOW);
    delayMicroseconds(6);
    buttons |= !digitalRead(DATA_SERIAL) << i;
    digitalWrite(DATA_CLOCK, HIGH);
    delayMicroseconds(6);
  }
}

bool input_left()  { return buttons & LEFT; }
bool input_right() { return buttons & RIGHT; }
bool input_up()    { return buttons & UP; }
bool input_down()  { return buttons & DOWN; }
bool input_fire()  { return buttons & Y; }
bool input_start() { return buttons & START; }

#else

// Analog joystick connections: VRx -> A0, VRy -> A1.
// Measured joystick resting values: X = 527, Y = 486.
static const int JOYSTICK_X_PIN = A0;
static const int JOYSTICK_Y_PIN = A1;
static const int JOYSTICK_CENTER_X = 527;
static const int JOYSTICK_CENTER_Y = 486;

// Ignore small variations around the resting position.
static const int JOYSTICK_DEADZONE = 120;
// Joystick SW button: D2 connected to GND when pressed.
static const uint8_t JOYSTICK_MENU_PIN = 2;

void input_setup() {
  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);

  // Independent fire button: D10 connected to GND when pressed.
  pinMode(K_FIRE, INPUT_PULLUP);

  // Use the Arduino's internal pull-up; no external resistor is needed.
  pinMode(JOYSTICK_MENU_PIN, INPUT_PULLUP);
}

bool input_left() {
  return analogRead(JOYSTICK_X_PIN) < (JOYSTICK_CENTER_X - JOYSTICK_DEADZONE);
}

bool input_right() {
  return analogRead(JOYSTICK_X_PIN) > (JOYSTICK_CENTER_X + JOYSTICK_DEADZONE);
}

// Default Y-axis convention: smaller ADC value = up, larger = down.
// If your joystick is mounted in the opposite orientation, swap these comparisons.
bool input_up() {
  return analogRead(JOYSTICK_Y_PIN) < (JOYSTICK_CENTER_Y - JOYSTICK_DEADZONE);
}

bool input_down() {
  return analogRead(JOYSTICK_Y_PIN) > (JOYSTICK_CENTER_Y + JOYSTICK_DEADZONE);
}

bool input_fire() {
  return digitalRead(K_FIRE) == LOW;
}

bool input_menu() {
  return digitalRead(JOYSTICK_MENU_PIN) == LOW;
}
#endif
