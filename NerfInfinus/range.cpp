#include "range.h"

Range::Range(){
  this->min = 0.0;
  this->max = 1.0;
  this->magnitude = 1.0;
}

Range::Range(float min, float max) {

  this->min = min;
  this->max = max;
  this->magnitude = this->max - this->min;
}

float Range::minVal() {
  return this->min;
}

float Range::maxVal() {
  return this->max;
}

float Range::normalize(float val) {
  val = this->clip(val);
  return (val - this->min)/(this->max - this->min);
}

bool Range::contains(float val) {
  return (this->min <= val) && (val <= this->max);
}

float Range::clip(float val) {
  if (val < this->min) {
    return this->min;
  }
  return (val <= this->max) ? val : this->max;
}

float Range::interpolate(float scalar) {
  return (this->magnitude * scalar) + this->min;
}
