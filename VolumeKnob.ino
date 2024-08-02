#include "AiEsp32RotaryEncoder.h"
#include <BfButton.h>
#include "BearRGBLed.h"
#include "RGBColor.h"
#include <BleKeyboard.h>

const long RIGHT_LEFT_DELAY = 100;
const long BLINK_DELAY = 250;
const long LONG_PRSS_DELAY = 3000;

const int buttonPin = 4;
const int redPin = 5;
const int greenPin = 6;
const int bluPin = 7;

long previousPos = 0;
bool pressedRotation = false;
bool ignoreSingleClick= false;
bool paringRunnig = false;

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

BfButton encoderButton(BfButton::STANDALONE_DIGITAL, buttonPin, true, LOW);
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(2, 3, buttonPin, -1, 4);
BearRGBLed rgbLed(redPin, greenPin, bluPin);
BleKeyboard bleKeyboard("BearKnob");

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  initRotaryEncoder();

  encoderButton.onPress(pressHandler)
    .onDoublePress(pressHandler)
    .onPressFor(pressHandler, LONG_PRSS_DELAY);
  bleKeyboard.begin();
}

void loop() {
  if (currentMode == PARING && !paringRunnig) {
    beginParing();
  } 

  checkRotation();
  encoderButton.read();
  rgbLed.keepBlinking();
}

void initRotaryEncoder() {
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-100000000, 100000000, true); 
  rotaryEncoder.disableAcceleration();
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
  if (ignoreSingleClick) {
    ignoreSingleClick = false;
    return;
  }
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" Single clicked.");
       
      if (currentMode == PARING) {
        endParing();
        currentMode = VOLUME_SCREEN;
        rgbLed.on(colors[currentMode]);
      } else {
        bleKeyboard.print("p");
      }
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double clicked.");
      {
        Mode next = modes[currentMode + 1];
        if (next == PARING) {
          currentMode = VOLUME_SCREEN;
        } else {
          currentMode = next;
        }
      }
      rgbLed.on(colors[currentMode]);
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      beginParing();
      break;
  }
}

void checkRotation() {

  if (pressedRotation && !encoderButtonPressed()) {
    pressedRotation = false;
    ignoreSingleClick = true;
  }

  if (rotaryEncoder.encoderChanged()) {

    long pos = rotaryEncoder.readEncoder();
    pressedRotation = encoderButtonPressed();

    String key = "null";

    if (pos > previousPos) {
      key = pressedRotation ? "t" : "r";
    } else {
      key = pressedRotation ? ";" : "l";
    }
    bleKeyboard.print(key);
    
    previousPos = pos;

    if (pressedRotation) Serial.print("Pressed ");

    Serial.print("Rotation : ");
    Serial.println(key);
  }
}

bool encoderButtonPressed() {
  return !digitalRead(buttonPin);
}