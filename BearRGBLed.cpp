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
  pinMode(_redPin, OUTPUT);
  pinMode(_greenPin, OUTPUT);
  pinMode(_bluPin, OUTPUT);
}

void BearRGBLed::blink(long blinkRate, long time, RGBColor color) {
  _blinkColor = color;
  _blinkRate = blinkRate;
  _blinking = true;
  _startBlinkMillis = millis();
  on(_blinkColor);
}

void BearRGBLed::blink(long blinkRate, RGBColor color) {
  _blinkColor = color;
  _blinkRate = blinkRate;
  _blinking = true;
  _startBlinkMillis = millis();
  on(_blinkColor);
}

void BearRGBLed::on(RGBColor color) {
  _onTimeStart = 0;
  _onTime = 0;
  _mainColor = color;
  on(0, color);
}

void BearRGBLed::on(long time, RGBColor color) {
  if (time > 0) {
    _onTime = time;
    _onTimeStart = millis();
  }

  analogWrite(_redPin, color.red());
  analogWrite(_greenPin, color.green());
  analogWrite(_bluPin, color.blu());
}

void BearRGBLed::light() {
  keepBlinking();
  keepOn();
}


void BearRGBLed::keepOn() {
  Serial.print("keepOn : ");
  Serial.println(_onTime);
  if (_onTime > 0 && (millis() - _onTimeStart > _onTime)) {
    on(_mainColor);
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
      on(_blinkColor);
    } else {
      off();
    }
  }
}

void BearRGBLed::stopBlinking() {
  _blinkRate = 0;
  _blinking = false;
  _startBlinkMillis = 0;
  _blinkColor = RGBColor(0, 0, 0);
  off();
}

void BearRGBLed::off() {
  on(_zero);
}