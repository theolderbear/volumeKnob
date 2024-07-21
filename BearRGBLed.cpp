#include "HardwareSerial.h"
#include "Arduino.h"
#include "BearRGBLed.h"
#include "RGBColor.h"

RGBColor zero(0, 0, 0);

BearRGBLed::BearRGBLed(int redPin, int greenPin, int bluPin) {
  _redPin = redPin;
  _greenPin = greenPin;
  _bluPin = bluPin;
  _blinkRate = 0;
  _blinking = false;
  _blinkColor = RGBColor(0, 0, 0);
  pinMode(_redPin, OUTPUT);
  pinMode(_greenPin, OUTPUT);
  pinMode(_bluPin, OUTPUT);
}

void BearRGBLed::blink(int blinkRate, RGBColor color) {
  _blinkColor = color;
  _blinkRate = blinkRate;
  _blinking = true;
  _startBlinkMillis = millis();
}

void BearRGBLed::keepBlinking() {
  if (_blinkRate == 0) {
    return;
  }
  if (millis() - _startBlinkMillis >= _blinkRate) {
    _blinking = !_blinking;
    _startBlinkMillis = millis();
  }
  if (_blinking) {
    on(_blinkColor);
  } else {
    off();
  }
}

void BearRGBLed::stopBlinking() {
  _blinkRate = 0;
  _blinking = false;
  _startBlinkMillis = 0;
  _blinkColor = RGBColor(0, 0, 0);
  off();
}

void BearRGBLed::on(RGBColor color) {
  analogWrite(_redPin, color.red());
  analogWrite(_greenPin, color.green());
  analogWrite(_bluPin, color.blu());
}

void BearRGBLed::off() {
  on(zero);
}