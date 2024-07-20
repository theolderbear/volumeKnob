#include "RGBColor.h"

RGBColor::RGBColor(int red, int green, int blu) {
  _red = red;
  _green = green;
  _blu = blu;
}

int RGBColor::red() {
  return _red;
}

int RGBColor::green() {
  return _green;
}

int RGBColor::blu() {
  return _blu;
}