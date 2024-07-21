#include <Encoder.h>
#include <BfButton.h>
#include "BearRGBLed.h"

const long RIGHT_LEFT_DELAY = 100;
const long PARING_DELAY = 2500;
const int BLINK_DELAY = 250;

int buttonPin = 4;

long oldPos = 0;
long lastRotation = 0;
long paringMillis = 0;
unsigned int paringCount;

boolean paring = false;

String RIGHT = "RIGHT";
String LEFT = "LEFT";

boolean rotationRunning = false;

BfButton btn(BfButton::STANDALONE_DIGITAL, 4, true, LOW);
Encoder enc(2, 3);
BearRGBLed rgbLed(5, 6, 7);
RGBColor blu(0, 0, 255);

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  btn.onPress(pressHandler)
    .onDoublePress(pressHandler)  // default timeout
    .onPressFor(pressHandler, 1000);
}

void loop() {
  rotation();
  btn.read();

  rgbLed.keepBlinking();
}

void setLedColor() {
  if (paring) {
    rgbLed.setColor(blu);
  } else {
    rgbLed.stopBlinking();
  }
}

void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern) {
  Serial.print(btn->getID());
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
      if (!paring) {
        if (paringCount == 0) {
          paringMillis = millis();
        }
        paringCount++;
        if (millis() - paringMillis > PARING_DELAY || paringCount > 3) {
          paringCount = 0;
        }
      } else {
        paringCount = 0;
        paring = false;
        rgbLed.stopBlinking();
      }
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double pressed.");

      break;
    case BfButton::LONG_PRESS:
      Serial.print(" long pressed.");
      Serial.println(paringCount);

      paring = true;
      rgbLed.blink(BLINK_DELAY, blu);
      if (paringCount == 3) {
        paring = true;
        
      }
      break;
  }
}

void rotation() {

  if (!rotationRunning) {
    rotationRunning = true;

    long pos = enc.read();

    if (pos == oldPos || pos % 2 == 0 || millis() - lastRotation < RIGHT_LEFT_DELAY) {
      endRotation(pos);
      return;
    }


    lastRotation = millis();
    String command = pos > oldPos ? RIGHT : LEFT;
    endRotation(pos);

    Serial.print("Direction : ");
    Serial.println(command);
  }
}

void endRotation(long pos) {
  rotationRunning = false;
  oldPos = pos;
}