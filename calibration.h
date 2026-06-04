#pragma once
#include <Arduino.h>
#include "config.h"

struct CalibrationData {
  int offsets[4][3]; // 4 legs, 3 joints each
  char wifiSSID[32];
  char wifiPass[64];
  
  void setDefaults();
};

extern CalibrationData calibration;

bool loadCalibration();
bool saveCalibration();