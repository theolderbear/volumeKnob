#include <BfButton.h>
#include "BearRGBLed.h"
#include "RGBColor.h"
#include <BleKeyboard.h>
#include "WiFi.h"
#include "esp_wifi.h"

const long RIGHT_LEFT_DELAY = 100;
const long BLINK_DELAY = 250;
const long LONG_PRSS_DELAY = 1000;
const long PARING_TIME = 10000;
const long BATTERY_LEVEL_TIME = 5 * 60 * 1000;  // How often to check battery level
const long CRITICAL_BATTERY_LEVEL = 10;
const long LOW_BATTERY_LEVEL = 30;
const long BATTERY_DELAY = 2000;  // How long the low battery alert should be held

const int rotaryPinA = 3;
const int rotaryPinB = 2;
const int buttonPin = 4;
const int redPin = 5;
const int greenPin = 6;
const int bluPin = 7;
const int batteryPin = A0;

const bool batteryLevelShowVolts = true;
const bool printBatteryStatus = false;

const float MAX_BATTERY_V = 8.40;
const float MAX_BATTERY_V_DELTA = 1.2;
const float MAX_PIN_VOLTAGE = 3.3;  // Max voltage that can be read from a pin

const unsigned int MAX_ANALOG_READ = 4095;
const unsigned int R1_V_DIVIDER = 5000;
const unsigned int R2_V_DIVIDER = 5000;

String rotationTestMessage = "";

long batteryLevelMillis = 0;
long paringStartMills = 0;
long previousPos = 0;
bool pressedRotation = false;
bool ignoreSingleClick = false;
bool paringRunnig = false;

enum Mode {
  VOLUME_SCREEN = 0,
  OTHER = 1,
  PARING = 2,
  OFF = 3
};

RGBColor modeColors[OFF + 1] = {
  RGBColor(0, 0, 255),  // Volume
  RGBColor(0, 255, 0),  // Green
  RGBColor(0, 0, 255),  // Paring
  RGBColor(0, 0, 0)     // Off
};

RGBColor RED(255, 0, 0);
RGBColor ORANGE(255, 128, 0);

Mode modes[OFF + 1] = { VOLUME_SCREEN, OTHER, PARING, OFF };
Mode currentMode = OFF;

BfButton encoderButton(BfButton::STANDALONE_DIGITAL, buttonPin, true, LOW);
BearRGBLed rgbLed(redPin, greenPin, bluPin);
BleKeyboard bleKeyboard("BearKnob");

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(rotaryPinA, INPUT);
  pinMode(rotaryPinB, INPUT);

  encoderButton.onPress(pressHandler)
    .onDoublePress(pressHandler)
    .onPressFor(pressHandler, LONG_PRSS_DELAY);

  beginParing();
  bleKeyboard.begin();
}

void loop() {
  stopWifi();

  scheduledLog();
  printRotationStatus();
  setBatteryLevel();
  bleConnection();
  checkRotation();
  encoderButton.read();
  rgbLed.light();
}

void setBatteryLevel() {
  if (bleKeyboard.isConnected() && (batteryLevelMillis == 0 || (millis() - batteryLevelMillis > BATTERY_LEVEL_TIME))) {

    int batteryLevel = getBatteryPercentage();

    if (batteryLevel < CRITICAL_BATTERY_LEVEL) {
      rgbLed.on(BATTERY_DELAY, RED);
    } else if (batteryLevel < LOW_BATTERY_LEVEL) {
      rgbLed.on(BATTERY_DELAY, ORANGE);
    }

    bleKeyboard.setBatteryLevel(batteryLevel);
    batteryLevelMillis = millis();
  }
}

int getBatteryPercentage() {

  int dividePin = analogRead(batteryPin);
  float divideV = dividePin * MAX_PIN_VOLTAGE / MAX_ANALOG_READ;
  float v = divideV * (R1_V_DIVIDER + R2_V_DIVIDER) / R2_V_DIVIDER;
  int batteryLevel = 100 / (MAX_BATTERY_V - MAX_BATTERY_V_DELTA) * (v - MAX_BATTERY_V_DELTA);

  if (batteryLevelShowVolts) {
    batteryLevel = v * 10;
  }


  if (batteryLevel < 0) {
    batteryLevel = 0;
  }

  printBatteryLevel(dividePin, divideV, v, batteryLevel);
  return batteryLevel;
}

void bleConnection() {
  if (currentMode != OFF && currentMode != PARING && !bleKeyboard.isConnected()) {
    beginParing();
  }

  if (currentMode == PARING && (bleKeyboard.isConnected() || (millis() - paringStartMills > PARING_TIME))) {
    endParing();
  }
}

void beginParing() {
  Serial.println("Connecting.......");

  paringStartMills = millis();
  currentMode = PARING;
  rgbLed.blink(BLINK_DELAY, modeColors[currentMode]);
}

void endParing() {
  if (bleKeyboard.isConnected()) {
    Serial.println("Connected");
    currentMode = VOLUME_SCREEN;
  } else {
    Serial.println("Off");
    currentMode = OFF;
  }

  paringStartMills = 0;

  rgbLed.on(modeColors[currentMode]);
}

void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern) {
  if (currentMode == OFF) {
    beginParing();
    return;
  }
  if (pressedRotation) {
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
      bleKeyboard.print("m");
      break;
    case BfButton::LONG_PRESS:
      {
        Mode next = modes[currentMode + 1];
        if (next == PARING) {
          currentMode = VOLUME_SCREEN;
        } else {
          currentMode = next;
        }
      }
      rgbLed.on(modeColors[currentMode]);
      break;
  }
}

void checkRotation() {

  if (pressedRotation && !encoderButtonPressed()) {
    pressedRotation = false;
    ignoreSingleClick = true;
  }

  long pos = digitalRead(rotaryPinA);

  if (pos != previousPos && currentMode == OFF) {
    beginParing();
  }

  if (pos != previousPos) {
    String key = "null";
    pressedRotation = encoderButtonPressed();
    if (digitalRead(rotaryPinB) == pos) {
      key = pressedRotation ? "t" : (currentMode == VOLUME_SCREEN ? "r" : "g");
    } else {
      key = pressedRotation ? ";" : (currentMode == VOLUME_SCREEN ? "l" : "h");
    }
    bleKeyboard.print(key);
  }
  previousPos = pos;
}

bool encoderButtonPressed() {
  return !digitalRead(buttonPin);
}

void printRotationStatus() {

  if (printBatteryStatus) return;

  String currentRotationTestMessage = "" + String(digitalRead(rotaryPinA)) + " : " + String(digitalRead(rotaryPinB));
  // Serial.println(currentRotationTestMessage);
  if (!rotationTestMessage.equals(currentRotationTestMessage)) {
    Serial.println(currentRotationTestMessage);
    rotationTestMessage = currentRotationTestMessage;
  }
}

long scheduledLogMillis = 0;
void scheduledLog() {
  if (millis() - scheduledLogMillis > 5000) {
    Serial.print("Mode : ");
    Serial.println(currentMode);
    scheduledLogMillis = millis();
  }
}

void printBatteryLevel(int dividePin, float divideV, float v, int batteryLevel) {
  Serial.print("Battery, pin : ");
  Serial.print(dividePin);
  Serial.print(", dv : ");
  Serial.print(divideV);
  Serial.print(", v : ");
  Serial.print(v);
  Serial.print(", % : ");
  Serial.println(batteryLevel);
}

void stopWifi() {
  WiFi.mode(WIFI_OFF);
  esp_wifi_stop();
}