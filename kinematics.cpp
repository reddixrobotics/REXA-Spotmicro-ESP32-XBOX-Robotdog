#include "kinematics.h"
#include <math.h>
#include "sensors.h"
#include "gait.h"

float legX[4];
float legZ[4];
int standZ = STAND_HEIGHT; // initialized to default standing height from config.h

bool solveIK(float x_fwd, float z_down, float &fm, float &tm) {
  float L2 = x_fwd * x_fwd + z_down * z_down;
  float L  = sqrtf(L2);
  
  // Bound check for hardware dimensions and reachability
  if (L > MAX_REACH) return false;
  if (L < fabsf(FEMUR_LEN - TIBIA_LEN) + 1.0f) return false;

  // Law of Cosines for Knee (Tibia) Joint Angle
  float cosK = (FEMUR_LEN * FEMUR_LEN + TIBIA_LEN * TIBIA_LEN - L2) / (2.0f * FEMUR_LEN * TIBIA_LEN);
  cosK = constrain(cosK, -1.0f, 1.0f);
  tm = (M_PI - acosf(cosK)) * (180.0f / M_PI);

  // Law of Cosines for Shoulder (Femur) Joint Angle relative to the Hip-to-Foot line
  float cosA = (FEMUR_LEN * FEMUR_LEN + L2 - TIBIA_LEN * TIBIA_LEN) / (2.0f * FEMUR_LEN * L);
  cosA = constrain(cosA, -1.0f, 1.0f);
  
  // Angle of Hip-to-Foot line relative to the sagittal x-axis
  float gamma = atan2f(z_down, x_fwd);
  
  // Solve for backward-bending knee configuration (knee-backward / elbow-up)
  fm = (gamma + acosf(cosA)) * (180.0f / M_PI);
  
  return true;
}

bool moveFoot(uint8_t leg, float x_fwd, float z_down, float coxaRight) {
  // Inject active IMU stabilization offset during standing or walking states
  if (motionState == ST_STAND || motionState == ST_WALK) {
    z_down += getStabilizationOffsetZ(leg);
  }

  float fm, tm;
  if (!solveIK(x_fwd, z_down, fm, tm)) return false;
  
  setTgt(leg, CX, coxaServo(leg, coxaRight));
  setTgt(leg, FM, femurServo(leg, fm));
  setTgt(leg, TB, tibiaServo(leg, tm));
  return true;
}

bool snapFoot(uint8_t leg, float x_fwd, float z_down, float coxaRight) {
  // Inject active IMU stabilization offset during standing or walking states
  if (motionState == ST_STAND || motionState == ST_WALK) {
    z_down += getStabilizationOffsetZ(leg);
  }

  float fm, tm;
  if (!solveIK(x_fwd, z_down, fm, tm)) return false;
  
  snapS(leg, CX, coxaServo(leg, coxaRight));
  snapS(leg, FM, femurServo(leg, fm));
  snapS(leg, TB, tibiaServo(leg, tm));
  return true;
}

bool moveFootRawCox(uint8_t leg, float x_fwd, float z_down, int rawCox) {
  float fm, tm;
  if (!solveIK(x_fwd, z_down, fm, tm)) return false;
  
  setTgt(leg, CX, constrain(rawCox, 0, 180));
  setTgt(leg, FM, femurServo(leg, fm));
  setTgt(leg, TB, tibiaServo(leg, tm));
  return true;
}

float liftArc(float t) {
  // Swing phase foot lift: smooth sine wave peaking at standZ - LIFT_H
  return standZ - LIFT_H * sinf(t * M_PI);
}
