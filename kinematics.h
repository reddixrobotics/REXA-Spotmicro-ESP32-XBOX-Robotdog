#pragma once
#include <Arduino.h>
#include "config.h"
#include "servo_driver.h"

// Foot positions tracking
extern float legX[4];
extern float legZ[4];
extern int standZ;

// Kinematics solvers & foot controllers
bool solveIK(float x_fwd, float z_down, float &fm, float &tm);
bool moveFoot(uint8_t leg, float x_fwd, float z_down, float coxaRight = 0.0f);
bool snapFoot(uint8_t leg, float x_fwd, float z_down, float coxaRight = 0.0f);
bool moveFootRawCox(uint8_t leg, float x_fwd, float z_down, int rawCox);
float liftArc(float t);
