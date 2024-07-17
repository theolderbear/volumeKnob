#include <Encoder.h>

const long RIGHT_LEFT_DELAY = 100;
Encoder enc(2, 3);

long oldPos = 0;
long lastRotation = 0;

String RIGHT = "RIGHT";
String LEFT = "LEFT";

boolean ratationRunning = false;

void setup() {
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);
}

void loop() {
  rotation();

  if (!digitalRead(4)) Serial.println("Clicked");
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