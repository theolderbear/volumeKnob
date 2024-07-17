#include <Encoder.h>
#include <BfButton.h>

const long RIGHT_LEFT_DELAY = 100;
Encoder enc(2, 3);

long oldPos = 0;
long lastRotation = 0;

String RIGHT = "RIGHT";
String LEFT = "LEFT";

BfButton btn(BfButton::STANDALONE_DIGITAL, 4, true, LOW);

boolean ratationRunning = false;

void setup() {
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);
  btn.onPress(pressHandler)
    .onDoublePress(pressHandler)  // default timeout
    .onPressFor(pressHandler, 1000);
}

void loop() {
  rotation();
  btn.read();
}

void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern) {
  Serial.print(btn->getID());
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double pressed.");
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      break;
  }
}

void rotation() {

  if (!ratationRunning) {
    ratationRunning = true;

    long pos = enc.read();

    if (pos == oldPos || pos % 2 == 0 || millis() - lastRotation < RIGHT_LEFT_DELAY) {
      endRotation(pos);
      return;
    }
    // Serial.println(pos);

    lastRotation = millis();
    String command = pos > oldPos ? RIGHT : LEFT;
    endRotation(pos);

    Serial.print("Direction : ");
    Serial.println(command);
  }
}

void endRotation(long pos) {
  ratationRunning = false;
  oldPos = pos;
}