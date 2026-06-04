#include "gait.h"

// Initialize gait and motion state variables
int hLevel = 2; // Default to middle level (205mm)
float legPhase[4] = {0.0f, 0.5f, 0.0f, 0.5f};
bool gaitRunning = false;

float joyX = 0.0f;
float joyY = 0.0f;
float joyMag = 0.0f;

float phaseInc = 0.0f;
float walkDir = 1.0f;
float prevWalkDir = 1.0f;
float turnAmt = 0.0f;
bool spotTurn = false;

MotionState motionState = ST_STAND;

// External helper to run server/Xbox polling in wait loops
extern void handleNetworkAndBluetooth();

void waitMs(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    updateServos();
    handleNetworkAndBluetooth();
    yield();
    delay(LOOP_MS);
  }
}

void initGait() {
  standZ = HEIGHT_Z[hLevel];
  for (int leg = 0; leg < 4; leg++) {
    legX[leg] = LEAN_X;
    legZ[leg] = standZ;
    snapFoot(leg, LEAN_X, standZ, 0.0f);
  }
  
  // Phase offsets for diagonal trot
  // For backward walks, offset by 0.5 to reverse the footfall order
  float bOff = (walkDir < 0.0f) ? 0.5f : 0.0f;
  legPhase[RF] = fmodf(0.0f + bOff, 1.0f);
  legPhase[BL] = fmodf(0.0f + bOff, 1.0f);
  legPhase[BR] = fmodf(0.5f + bOff, 1.0f);
  legPhase[FL] = fmodf(0.5f + bOff, 1.0f);
  gaitRunning = true;
}

void stepGait() {
  stepDeg = STEP_WALK;

  for (int leg = 0; leg < 4; leg++) {
    float ph = legPhase[leg];
    bool isRightLeg = (leg == RF || leg == BR);

    if (spotTurn) {
      // ── SPOT TURN: FRONTAL D-ARC ─────────────────────────────
      // Coxa draws a D-shape viewed from front/back to rotate body on spot
      float sweepAmt = TURN_SWEEP * fabsf(turnAmt);
      float startSwing = isRightLeg ?  sweepAmt * (turnAmt > 0 ? 1.0f : -1.0f)
                                    : -sweepAmt * (turnAmt > 0 ? 1.0f : -1.0f);
      float endSwing = -startSwing;

      if (ph < 0.5f) {
        // GROUND PHASE: coxa linear sweep (foot pushes ground laterally)
        float t = ph / 0.5f;
        float coxRight = startSwing + (endSwing - startSwing) * t;
        moveFoot(leg, LEAN_X, standZ, coxRight);
        legX[leg] = LEAN_X;
        legZ[leg] = standZ;
      } else {
        // AIR PHASE: swing phase (femur/tibia lift foot, coxa resets to start swing)
        float t = (ph - 0.5f) / 0.5f;
        float fz = liftArc(t);
        float coxRight = endSwing + (startSwing - endSwing) * t;
        moveFoot(leg, LEAN_X, fz, coxRight);
        legX[leg] = LEAN_X;
        legZ[leg] = fz;
      }
    } else {
      // ── NORMAL TROT WALK (forward, backward, curve turn) ─────
      // Differential stride for curve turning (right legs shorter, left longer for right turn)
      float curveFactor = 1.0f;
      if (fabsf(turnAmt) > 0.05f) {
        float bias = turnAmt * 0.55f;
        curveFactor = isRightLeg ? (1.0f - bias) : (1.0f + bias);
        curveFactor = constrain(curveFactor, 0.10f, 1.90f);
      }
      
      float effStride = STRIDE * curveFactor;
      float dir = walkDir; // +1 for fwd, -1 for bwd

      if (ph < 0.5f) {
        // GROUND PHASE: foot sweeps back (relative to body) to propel robot forward
        float t = ph / 0.5f;
        float fx = LEAN_X + dir * effStride * (1.0f - 2.0f * t);
        moveFoot(leg, fx, standZ, 0.0f);
        legX[leg] = fx;
        legZ[leg] = standZ;
      } else {
        // AIR PHASE: foot lifts and swings forward to reset stance
        float t = (ph - 0.5f) / 0.5f;
        float fx = LEAN_X + dir * effStride * (2.0f * t - 1.0f);
        float fz = liftArc(t);
        moveFoot(leg, fx, fz, 0.0f);
        legX[leg] = fx;
        legZ[leg] = fz;
      }
    }

    // Advance phase
    legPhase[leg] += phaseInc;
    if (legPhase[leg] >= 1.0f) legPhase[leg] -= 1.0f;
  }
}

void updateGaitFromJoystick() {
  if (joyMag < 0.10f) {
    if (gaitRunning) {
      gaitRunning = false;
      poseStand(true);
    }
    return;
  }

  float newDir = (joyY >= 0.0f) ? 1.0f : -1.0f;
  float absX = fabsf(joyX);
  float absY = fabsf(joyY);
  
  // Spot turn triggers if joystick is far left/right and mostly horizontal
  bool newSpot = (joyMag > SPOT_THRESH) && (absX > absY * 1.3f);

  if (!gaitRunning) {
    walkDir = newDir;
    initGait();
    motionState = ST_WALK;
  } else {
    // Re-initialize phases if walk direction is reversed to keep gait clean
    if (!newSpot && !spotTurn && newDir != prevWalkDir) {
      walkDir = newDir;
      initGait();
    } else {
      walkDir = newDir;
    }
  }

  prevWalkDir = walkDir;
  spotTurn = newSpot;
  turnAmt = joyX;
  
  // Speed is proportional to joystick deflection magnitude
  phaseInc = 0.012f + joyMag * 0.020f;

  if (spotTurn) walkDir = 1.0f; // spot turn coxa sweep calculations expect positive walkDir
}

void poseStand(bool slow) {
  standZ = HEIGHT_Z[hLevel];

  if (slow && motionState == ST_SIT) {
    // ── STAND FROM SIT ────────────────────────────────────
    // Raise all four legs simultaneously for a balanced rise from flat sit
    stepDeg = STEP_MED;
    for (int leg = 0; leg < 4; leg++) {
      moveFoot(leg, LEAN_X, standZ, 0.0f);
    }
    waitMs(900);
  } else if (slow) {
    // Normal slow standing adjustment
    stepDeg = STEP_MED;
    for (int leg = 0; leg < 4; leg++) {
      moveFoot(leg, LEAN_X, standZ, 0.0f);
    }
    waitMs(500);
  } else {
    // Snap immediately (boot-up stance or urgent reset)
    stepDeg = STEP_WALK;
    for (int leg = 0; leg < 4; leg++) {
      snapFoot(leg, LEAN_X, standZ, 0.0f);
    }
  }

  for (int leg = 0; leg < 4; leg++) {
    legX[leg] = LEAN_X;
    legZ[leg] = standZ;
  }
  motionState = ST_STAND;
  gaitRunning = false;
  Serial.println(F("[POSE] Stand"));
}

void poseSit() {
  if (gaitRunning) gaitRunning = false;
  standZ = HEIGHT_Z[hLevel];
  stepDeg = STEP_MED;

  // Lower all four legs to squat flat (lay down position)
  float sitZ = 92.0f;
  float sitX = LEAN_X;

  // Move all legs simultaneously
  moveFoot(RF, sitX, sitZ, 0.0f);
  moveFoot(FL, sitX, sitZ, 0.0f);
  moveFoot(BR, sitX, sitZ, 0.0f);
  moveFoot(BL, sitX, sitZ, 0.0f);
  
  waitMs(900);

  for (int leg = 0; leg < 4; leg++) {
    legX[leg] = sitX;
    legZ[leg] = sitZ;
  }
  
  motionState = ST_SIT;
  Serial.println(F("[POSE] Sit"));
}

void applyHeight() {
  standZ = HEIGHT_Z[hLevel];
  stepDeg = STEP_SLOW;
  for (int leg = 0; leg < 4; leg++) {
    legX[leg] = LEAN_X;
    legZ[leg] = standZ;
    moveFoot(leg, LEAN_X, standZ, 0.0f);
  }
  Serial.print(F("[HEIGHT] Level: ")); Serial.print(hLevel);
  Serial.print(F(" -> Z: ")); Serial.println(standZ);
}

void doGreeting() {
  Serial.println(F("[GREET] Starting Wave..."));
  motionState = ST_GREET;
  gaitRunning = false;
  standZ = HEIGHT_Z[hLevel];

  // Phase 1: Bow down (back legs low squat, front legs push low forward)
  stepDeg = STEP_MED;
  moveFoot(BL, LEAN_X + 5.0f, 115.0f);
  moveFoot(BR, LEAN_X + 5.0f, 115.0f);
  moveFoot(FL, LEAN_X - 5.0f, standZ - 18.0f);
  moveFoot(RF, LEAN_X - 5.0f, standZ - 18.0f);
  waitMs(2200);

  // Phase 2: Lift and wave front-right paw (pure femur/tibia, coxa locked at neutral)
  stepDeg = STEP_SLOW;
  moveFoot(RF, LEAN_X, standZ - 55.0f, 0.0f); // Lift paw high
  waitMs(1100);

  // Wave 3 times
  for (int i = 0; i < 3; i++) {
    moveFoot(RF, LEAN_X, standZ - 20.0f, 0.0f); // Wave down
    waitMs(900);
    moveFoot(RF, LEAN_X, standZ - 55.0f, 0.0f); // Wave up
    waitMs(900);
  }

  // Restore RF to front bow position
  moveFoot(RF, LEAN_X - 5.0f, standZ - 18.0f, 0.0f);
  waitMs(800);

  // Phase 3: Smooth individual leg return to stand stance
  motionState = ST_GREET_RETURN;
  stepDeg = STEP_SLOW;
  moveFoot(RF, LEAN_X, standZ, 0.0f); waitMs(950);
  moveFoot(FL, LEAN_X, standZ, 0.0f); waitMs(950);
  moveFoot(BR, LEAN_X, standZ, 0.0f); waitMs(1300);
  moveFoot(BL, LEAN_X, standZ, 0.0f); waitMs(1500);

  stepDeg = STEP_WALK;
  for (int leg = 0; leg < 4; leg++) {
    legX[leg] = LEAN_X;
    legZ[leg] = standZ;
  }
  motionState = ST_STAND;
  Serial.println(F("[GREET] Stance Restored."));
}
