#pragma once
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include "config.h"
#include "calibration.h"

extern Adafruit_PWMServoDriver pwm;

// Async servo states
extern float stepDeg;
extern int sPos[4][3];
extern int sTgt[4][3];
extern bool sAct[4][3];

void initServos();
void updateServos();
void writeServo(uint8_t leg, uint8_t j);
void setTgt(uint8_t leg, uint8_t j, int t);
void snapS(uint8_t leg, uint8_t j, int v);
bool allDone(uint8_t leg);
bool allLegsDone();

// Mirroring and angle calculations
int coxaServo(uint8_t leg, float rightwardSwing);
int femurServo(uint8_t leg, float fm);
int tibiaServo(uint8_t leg, float tm);
