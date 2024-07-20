#ifndef BearRGBLed_h
#define BearRGBLed_h

#include "Arduino.h"
#include "RGBColor.h"

class BearRGBLed {
	public:
		BearRGBLed(int redPin, int greenPin, int bluPin);
		void blink(int blinkRate, RGBColor color);
    void setColor(RGBColor color);
    void off();
	private:
		int _redPin;
		int _greenPin;
		int _bluPin;
};
#endif
