#pragma once
#include <Arduino.h>
#include "config.h"
#include "kinematics.h"
#include "servo_driver.h"

// Gait & motion state variables
extern int hLevel;
extern float legPhase[4];
extern bool gaitRunning;

// Joystick control values
extern float joyX;
extern float joyY;
extern float joyMag;

// Gait parameters
extern float phaseInc;
extern float walkDir;
extern float prevWalkDir;
extern float turnAmt;
extern bool spotTurn;

extern MotionState motionState;

// Functions
void initGait();
void stepGait();
void updateGaitFromJoystick();
void poseStand(bool slow = false);
void poseSit();
void applyHeight();
void doGreeting();
void waitMs(unsigned long ms);
