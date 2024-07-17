#include <Encoder.h>

Encoder enc(2, 3);

long oldPos = 0;

String UP = "UP";
String DOWN = "DOWN";

void setup() {
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);
}

void loop() {
  upOrDown();

  if (!digitalRead(4)) Serial.println("Clicked");
}

void upOrDown() {
  long pos = enc.read();
  
  if (pos == oldPos  ) {
    return;
  }

  String command = pos > oldPos ? UP : DOWN;
  Serial.print("Direction : ");
  Serial.println(command);
  oldPos = pos;

}
