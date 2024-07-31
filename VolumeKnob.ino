#include "AiEsp32RotaryEncoder.h"
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
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(2, 3, buttonPin, -1, 4);
BearRGBLed rgbLed(5, 6, 7);

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    //set boundaries and if values should cycle or not
    //in this example we will set possible values between 0 and 1000;
    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, 1000, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

    /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
    //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
    rotaryEncoder.setAcceleration(250);
  
  encoderButton.onPress(pressHandler)
    .onDoublePress(pressHandler)
    .onPressFor(pressHandler, LONG_PRSS_DELAY);
}

void loop() {
  if (currentMode == PARING && !paringRunnig) {
    beginParing();
  } 

  checkRotation();
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
      Serial.println(" Single clicked.");
      
      if (currentMode == PARING) {
        endParing();
        currentMode = VOLUME_SCREEN;
        rgbLed.on(colors[currentMode]);
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

  if (!encoderButtonPressed()) {
    pressedRotation = false;
  }


  if (rotaryEncoder.encoderChanged())
    {
            // Serial.print("Value: ");
            // Serial.println(rotaryEncoder.readEncoder());
 

  if (!rotationLock) {
    rotationLock = true;

    long pos = rotaryEncoder.readEncoder();

    if (pos == oldPos || millis() - lastRotation < RIGHT_LEFT_DELAY) {
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
}

bool encoderButtonPressed() {
  return !digitalRead(buttonPin);
}

void rotationEnded(long pos) {
  rotationLock = false;
  oldPos = pos;
}