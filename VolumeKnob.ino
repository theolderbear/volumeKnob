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
bool pressedRotation = false;
unsigned int paringCount;

boolean paring = false;
String RIGHT = "RIGHT";
String LEFT = "LEFT";

boolean rotationLock = false;

BfButton encoderButton(BfButton::STANDALONE_DIGITAL, 4, true, LOW);
Encoder enc(2, 3);
BearRGBLed rgbLed(5, 6, 7);
RGBColor blu(0, 0, 255);

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);

  encoderButton.onPress(pressHandler)
    .onDoublePress(pressHandler)  // default timeout
    .onPressFor(pressHandler, 1000);
}

void loop() {
  checkRotation();
  encoderButton.read();
  rgbLed.keepBlinking();

  if (!encoderButtonPressed()) {
    pressedRotation = false;
  }
}

void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern) {
  Serial.print(btn->getID());
  if (pressedRotation) {
    return;
  }
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" single clicked.");
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
      Serial.println(" double clicked.");

      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");

      paring = true;
      rgbLed.blink(BLINK_DELAY, blu);
      break;
  }
}

void checkRotation() {

  if (!rotationLock) {
    rotationLock = true;

    long pos = enc.read();

    if (pos == oldPos || pos % 2 == 0 || millis() - lastRotation < RIGHT_LEFT_DELAY) {
      rotationEnded(pos);
      return;
    }

    pressedRotation = encoderButtonPressed();

    lastRotation = millis();
    String command = pos > oldPos ? RIGHT : LEFT;
    rotationEnded(pos);
    if (pressedRotation) Serial.print("Pressed ");
    Serial.print("Rotation : ");
    Serial.println(command);
  }
}

bool encoderButtonPressed() {
  return !digitalRead(buttonPin);
}

void rotationEnded(long pos) {
  rotationLock = false;
  oldPos = pos;
}