#include "servo_driver.h"
#include <Wire.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA_ADDR);

float stepDeg = STEP_WALK;
int sPos[4][3];
int sTgt[4][3];
bool sAct[4][3];

void initServos() {
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  delay(100);

  // Initialize with neutral angles (90 degrees)
  for (int leg = 0; leg < 4; leg++) {
    for (int j = 0; j < 3; j++) {
      sPos[leg][j] = 90;
      sTgt[leg][j] = 90;
      sAct[leg][j] = false;
    }
  }
}

void writeServo(uint8_t leg, uint8_t j) {
  // Apply calibration offset to the target angle
  int a = sPos[leg][j] + calibration.offsets[leg][j];
  a = constrain(a, 0, 180);
  
  // Convert angle to PCA9685 PWM pulse ticks
  uint16_t pulse = map(a, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(CH[leg][j], 0, pulse);
}

void setTgt(uint8_t leg, uint8_t j, int t) {
  t = constrain(t, 0, 180);
  if (abs(t - sPos[leg][j]) <= DEADBAND) {
    sPos[leg][j] = sTgt[leg][j] = t;
    sAct[leg][j] = false;
    writeServo(leg, j);
  } else {
    sTgt[leg][j] = t;
    sAct[leg][j] = true;
  }
}

void snapS(uint8_t leg, uint8_t j, int v) {
  v = constrain(v, 0, 180);
  sPos[leg][j] = sTgt[leg][j] = v;
  sAct[leg][j] = false;
  writeServo(leg, j);
}

void updateServos() {
  for (int leg = 0; leg < 4; leg++) {
    for (int j = 0; j < 3; j++) {
      if (!sAct[leg][j]) continue;
      int diff = sTgt[leg][j] - sPos[leg][j];
      if (abs(diff) <= DEADBAND) {
        sPos[leg][j] = sTgt[leg][j];
        sAct[leg][j] = false;
      } else {
        int step = max(1, (int)stepDeg);
        sPos[leg][j] += (diff > 0) ? step : -step;
      }
      writeServo(leg, j);
    }
  }
}

bool allDone(uint8_t leg) {
  return !sAct[leg][CX] && !sAct[leg][FM] && !sAct[leg][TB];
}

bool allLegsDone() {
  for (int i = 0; i < 4; i++) {
    if (!allDone(i)) return false;
  }
  return true;
}

int coxaServo(uint8_t leg, float rightwardSwing) {
  float s = IS_RIGHT[leg] ? rightwardSwing : -rightwardSwing;
  return constrain(COXA_ZERO[leg] + (int)roundf(s), 45, 135);
}

int femurServo(uint8_t leg, float fm) {
  int ang = IS_RIGHT[leg] ? (FEMUR_ZERO[leg] - ((int)roundf(fm) - 90))
                          : (FEMUR_ZERO[leg] + ((int)roundf(fm) - 90));
  return constrain(ang, 30, 150); // Safe limits to prevent going below horizontal
}

int tibiaServo(uint8_t leg, float tm) {
  return constrain(IS_RIGHT[leg] ? (TIBIA_ZERO[leg] + (int)roundf(tm))
                                 : (TIBIA_ZERO[leg] - (int)roundf(tm)), 0, 180);
}
