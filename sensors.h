#pragma once
#include <Arduino.h>
#include "config.h"

// IMU pitch/roll estimations (in degrees)
extern float bodyPitch;
extern float bodyRoll;
extern bool imuReady;

// Sensor initialization & updater
void initSensors();
void updateSensors();

// Active stabilization offsets helper
// Computes Z compensation offset in mm for a given leg based on body tilt
float getStabilizationOffsetZ(uint8_t leg);
