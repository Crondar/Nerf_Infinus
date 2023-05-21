#pragma once

struct Range {
  private:

  float min;
  float max;
  float magnitude;
  


  public:
  Range();
  Range(float min, float max);

  float minVal();
  float maxVal();
  float normalize(float);
  float clip(float);
  float interpolate(float);
  bool contains(float);
};