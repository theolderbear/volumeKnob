#include "esp32-hal.h"
#include "USBCDC.h"
#include "HardwareSerial.h"
#include "Arduino.h"
#include "BearRGBLed.h"
#include "RGBColor.h"

BearRGBLed::BearRGBLed(int redPin, int greenPin, int bluPin) {
  _redPin = redPin;
  _greenPin = greenPin;
  _bluPin = bluPin;
  _blinkRate = 0;
  _blinking = false;
  _blinkColor = _zero;
  _onTime = 0;
  pinMode(_redPin, OUTPUT);
  pinMode(_greenPin, OUTPUT);
  pinMode(_bluPin, OUTPUT);
}

void BearRGBLed::blink(long blinkRate, RGBColor color) {
  _mainColor = color;
  blink(blinkRate, 0, color);
}

void BearRGBLed::blink(long blinkRate, long time, RGBColor color) {
  reset();
  if (time > 0) {
    _onTime = time;
    _onTimeStart = millis();
  }

  _blinkColor = color;
  _blinkRate = blinkRate;
  _blinking = true;
  _startBlinkMillis = millis();
  set(color);
}

void BearRGBLed::on(RGBColor color) {
  _mainColor = color;
  on(0, color);
}

void BearRGBLed::on(long time, RGBColor color) {
  reset();
  if (time > 0) {
    _onTime = time;
    _onTimeStart = millis();
  }
  set(color);
}

void BearRGBLed::set(RGBColor color) {
  analogWrite(_redPin, color.red());
  analogWrite(_greenPin, color.green());
  analogWrite(_bluPin, color.blu());
}

void BearRGBLed::off() {
  set(_zero);
}

void BearRGBLed::light() {
  keepOn();
  keepBlinking();
}

void BearRGBLed::keepOn() {
  if (_onTime > 0 && (millis() - _onTimeStart > _onTime)) {
    reset();
    set(_mainColor);
  }
}

void BearRGBLed::keepBlinking() {
  if (_blinkRate == 0) {
    return;
  }
  if (millis() - _startBlinkMillis >= _blinkRate) {
    _blinking = !_blinking;
    _startBlinkMillis = millis();
    if (_blinking) {
      set(_blinkColor);
    } else {
      off();
    }
  }
}

void BearRGBLed::reset() {
  _onTimeStart = 0;
  _onTime = 0;
  _blinkRate = 0;
  _blinking = false;
  _startBlinkMillis = 0;
  _blinkColor = RGBColor(0, 0, 0);
}