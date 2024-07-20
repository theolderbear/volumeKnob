#include "Arduino.h"
#include "BearRGBLed.h"
#include "RGBColor.h"

BearRGBLed::BearRGBLed(int redPin, int greenPin, int bluPin) {
  _redPin = redPin;
  _greenPin = greenPin;
  _bluPin = bluPin;
  pinMode(_redPin, OUTPUT);
  pinMode(_greenPin, OUTPUT);
  pinMode(_bluPin, OUTPUT);
}

void BearRGBLed::blink(int blinkRate, RGBColor color) {
  analogWrite(_redPin, color.red());
  analogWrite(_greenPin, color.green());
  analogWrite(_bluPin, color.blu());
  delay(blinkRate);
  analogWrite(_redPin, 0);
  analogWrite(_greenPin, 0);
  analogWrite(_bluPin, 0);
  delay(blinkRate);
}

void BearRGBLed::setColor(RGBColor color) {
  analogWrite(_redPin, color.red());
  analogWrite(_greenPin, color.green());
  analogWrite(_bluPin, color.blu());
}

void BearRGBLed::off() {
  analogWrite(_redPin, 0);
  analogWrite(_greenPin, 0);
  analogWrite(_bluPin, 0);
}