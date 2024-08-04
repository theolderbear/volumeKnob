#include "AiEsp32RotaryEncoder.h"
#include <BfButton.h>
#include "BearRGBLed.h"
#include "RGBColor.h"
#include <BleKeyboard.h>

const long RIGHT_LEFT_DELAY = 100;
const long BLINK_DELAY = 250;
const long LONG_PRSS_DELAY = 1000;
const long PARING_TIME = 10000;
const long BATTERY_LEVEL_TIME = 5000;

const int buttonPin = 4;
const int redPin = 5;
const int greenPin = 6;
const int bluPin = 7;

long batteryLevelMillis = 0;
long paringStartMills = 0;
long previousPos = 0;
bool pressedRotation = false;
bool ignoreSingleClick= false;
bool paringRunnig = false;

enum Mode {
    VOLUME_SCREEN = 0,
    OTHER = 1,
    PARING = 2,
    OFF = 3
};

RGBColor colors[OFF + 1] = {
    RGBColor(0, 0, 255), // Volume
    RGBColor(0, 255, 0), // Red
    RGBColor(0, 0, 255), // Paring
    RGBColor(0, 0, 0)   // Off
};

Mode modes[OFF + 1] = {VOLUME_SCREEN, OTHER, PARING, OFF};
Mode currentMode = PARING;

BfButton encoderButton(BfButton::STANDALONE_DIGITAL, buttonPin, true, LOW);
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(2, 3, buttonPin, -1, 4);
BearRGBLed rgbLed(redPin, greenPin, bluPin);
BleKeyboard bleKeyboard("BearKnob");

void setup() {
  delay(2000);
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  initRotaryEncoder();

  encoderButton.onPress(pressHandler)
    .onDoublePress(pressHandler)
    .onPressFor(pressHandler, LONG_PRSS_DELAY);

  beginParing();
  bleKeyboard.begin();
  
}

void loop() {

  setBatteryLevel();

  if (currentMode != OFF && currentMode != PARING && !bleKeyboard.isConnected()) {
    beginParing();
  }

  if (currentMode == PARING && (bleKeyboard.isConnected() || (millis() - paringStartMills  > PARING_TIME))) {
    endParing();
  }

  checkRotation();
  encoderButton.read();
  rgbLed.keepBlinking();
}

int ct = 0;

void setBatteryLevel() {
  if (bleKeyboard.isConnected() && (batteryLevelMillis == 0 || (millis() - batteryLevelMillis > BATTERY_LEVEL_TIME))) {
    Serial.println("setBatteryLevel");
    bleKeyboard.setBatteryLevel(80 - ct*10);
    batteryLevelMillis = millis();
    ct = ct + 1;
  }
  
}

void IRAM_ATTR readEncoderISR() {
    rotaryEncoder.readEncoder_ISR();
}

void initRotaryEncoder() {
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-100000000, 100000000, true); 
  rotaryEncoder.disableAcceleration();
}

void beginParing() {
  Serial.println("Connecting...");
  
  paringStartMills = millis();
  currentMode = PARING;
  rgbLed.blink(BLINK_DELAY, colors[currentMode]);
}

void endParing() {
  Serial.print("Connected : ");
  // Serial.println(bleKeyboard.isConnected());
  // Serial.print("paringStartMills : ");
  // Serial.println(paringStartMills);
  // Serial.print("currentMode : ");
  // Serial.println(currentMode);
  if (bleKeyboard.isConnected()) {
    Serial.println("Connected");
    currentMode = VOLUME_SCREEN;
  } else {
    Serial.println("Off");
    currentMode = OFF;
  }

  paringStartMills = 0;
  rgbLed.stopBlinking();
  
  rgbLed.on(colors[currentMode]);
}

void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern) {
  if (currentMode == OFF) {
    beginParing();
    return;
  }
  if (pressedRotation ) {
    return;
  }
  if (ignoreSingleClick) {
    ignoreSingleClick = false;
    return;
  }
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      bleKeyboard.print("p");
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
      bleKeyboard.print("m");
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

     if (pos != previousPos && currentMode == OFF) {
      beginParing();
    }

    if (pos > previousPos) {
      key = pressedRotation ? "t" : "r";
    } else {
      key = pressedRotation ? ";" : "l";
    }
    bleKeyboard.print(key);
    
    previousPos = pos;
  }
}

bool encoderButtonPressed() {
  return !digitalRead(buttonPin);
}