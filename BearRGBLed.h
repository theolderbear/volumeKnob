#ifndef BearRGBLed_h
#define BearRGBLed_h

#include "Arduino.h"
#include "RGBColor.h"

class BearRGBLed {
	public:
		BearRGBLed(int redPin, int greenPin, int bluPin);
		void blink(int blinkRate, RGBColor color);
    void keepBlinking();
    void on(RGBColor color);
    void off();
    void stopBlinking();
	private:
		int _redPin;
		int _greenPin;
		int _bluPin;
    unsigned long _blinkRate;
    unsigned long _startBlinkMillis;
    bool _blinking = false;
    RGBColor _blinkColor = RGBColor(0, 0, 0);;
};
#endif
