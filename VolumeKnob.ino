#include <Encoder.h>
#include <BfButton.h>
#include "BearRGBLed.h"
#include "RGBColor.h"

const long RIGHT_LEFT_DELAY = 100;
const long BLINK_DELAY = 250;
const long LONG_PRSS_DELAY = 3000;

int buttonPin = 4;

long oldPos = 0;
long lastRotation = 0;
long paringMillis = 0;
bool pressedRotation = false;
bool paringRunnig = false;

String RIGHT = "RIGHT";
String LEFT = "LEFT";

enum Mode {
    VOLUME_SCREEN = 0,
    OTHER = 1,
    PARING = 2
};

RGBColor colors[PARING + 1] = {
    RGBColor(0, 254, 0),
    RGBColor(254, 0, 0),
    RGBColor(0, 0, 254)
};

Mode modes[PARING + 1] = {VOLUME_SCREEN, OTHER, PARING};
Mode currentMode = PARING;

boolean rotationLock = false;

BfButton encoderButton(BfButton::STANDALONE_DIGITAL, 4, true, LOW);
// Encoder enc(2, 3);
BearRGBLed rgbLed(5, 6, 7);


void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  
  encoderButton.onPress(pressHandler)
    .onDoublePress(pressHandler)
    .onPressFor(pressHandler, LONG_PRSS_DELAY);
}

void loop() {
  if (currentMode == PARING && !paringRunnig) {
    beginParing();
  } 

  // checkRotation();
  encoderButton.read();
  rgbLed.keepBlinking();
}

void beginParing() {
  paringRunnig=true;
  currentMode = PARING;
  rgbLed.blink(BLINK_DELAY, colors[PARING]);
}

void endParing() {
  paringRunnig=false;
  rgbLed.stopBlinking();
  
}

void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern) {
  Serial.print(btn->getID());
  if (pressedRotation) {
    return;
  }
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" single clicked.");
      
      if (currentMode == PARING) {
        endParing();
        currentMode = VOLUME_SCREEN;
        rgbLed.on(colors[currentMode]);
      }
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double clicked.");
      {

        Serial.print("currentMode : ");
        Serial.println(currentMode);
        Serial.print("next : ");
      
        Mode next = modes[currentMode + 1];
        if (next == PARING) {
          currentMode = VOLUME_SCREEN;
        } else {
          currentMode = next;
        }
      }
      rgbLed.on(colors[currentMode]);
      Serial.print("Mode : ");
      Serial.println(currentMode);
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      beginParing();
      break;
  }
}

// void checkRotation() {

//   if (!encoderButtonPressed()) {
//     pressedRotation = false;
//   }

//   if (!rotationLock) {
//     rotationLock = true;

//     long pos = enc.read();

//     if (pos == oldPos || pos % 2 == 0 || millis() - lastRotation < RIGHT_LEFT_DELAY) {
//       rotationEnded(pos);
//       return;
//     }

//     pressedRotation = encoderButtonPressed();

//     lastRotation = millis();
//     String command = pos > oldPos ? RIGHT : LEFT;
//     rotationEnded(pos);
//     if (pressedRotation) Serial.print("Pressed ");
//     Serial.print("Rotation : ");
//     Serial.println(command);
//   }
// }

bool encoderButtonPressed() {
  return !digitalRead(buttonPin);
}

// void rotationEnded(long pos) {
//   rotationLock = false;
//   oldPos = pos;
// }