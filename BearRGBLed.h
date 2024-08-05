#ifndef BearRGBLed_h
#define BearRGBLed_h

#include "Arduino.h"
#include "RGBColor.h"

class BearRGBLed {
public:
  BearRGBLed(int redPin, int greenPin, int bluPin);
  void blink(long blinkRate, RGBColor color);
  void on(RGBColor color);
  void blink(long blinkRate, long time, RGBColor color);
  void on(long time, RGBColor color);
  void off();
  void light();
  void keepBlinking();
  void stopBlinking();
private:
  int _redPin;
  int _greenPin;
  int _bluPin;
  unsigned long _blinkRate;
  unsigned long _startBlinkMillis;
  unsigned long _onTime;
  unsigned long _onTimeStart;
  bool _blinking = false;
  RGBColor _zero = RGBColor(0, 0, 0);
  RGBColor _mainColor = RGBColor(0, 0, 0);
  RGBColor _blinkColor = RGBColor(0, 0, 0);
  void keepOn();
  void reset();
  void set(RGBColor color);
};
#endif
