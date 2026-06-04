#include "xbox_control.h"
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

// Controller Parser Instance
// Leaving MAC empty allows pairing with any Xbox controller in pairing mode
XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

bool xboxConnected = false;

// Debouncing state trackers
static bool prevBtnA = false;
static bool prevBtnB = false;
static bool prevBtnX = false;
static bool prevBtnDirUp = false;
static bool prevBtnDirDown = false;

void initXbox() {
  Serial.println(F("[XBOX] Initializing BLE gamepad receiver..."));
  xboxController.begin();
}

void updateXbox() {
  xboxController.onLoop();
  
  if (xboxController.isConnected()) {
    if (!xboxConnected) {
      Serial.println(F("[XBOX] Controller CONNECTED!"));
      xboxConnected = true;
    }

    // 1. Read Left Stick Axes
    // joyLHori / joyLVert are uint16_t [0 - 65535] with ~32768 as neutral center
    uint16_t rawX = xboxController.xboxNotif.joyLHori;
    uint16_t rawY = xboxController.xboxNotif.joyLVert;

    // Convert to float [-1.0, 1.0]
    float fx = ((float)rawX - 32768.0f) / 32768.0f;
    float fy = -(((float)rawY - 32768.0f) / 32768.0f); // Invert Y so up is positive fwd

    // Apply joystick deadband (0.15) to filter center jitter
    float mag = sqrtf(fx * fx + fy * fy);
    if (mag < 0.15f) {
      joyX = 0.0f;
      joyY = 0.0f;
      joyMag = 0.0f;
    } else {
      // Normalize or scale linearly past the deadband
      joyX = fx;
      joyY = fy;
      joyMag = mag;
    }

    // 2. Read Buttons with Rising-Edge Detection (debouncing)
    bool curBtnA = xboxController.xboxNotif.btnA;
    bool curBtnB = xboxController.xboxNotif.btnB;
    bool curBtnX = xboxController.xboxNotif.btnX;
    bool curBtnDirUp = xboxController.xboxNotif.btnDirUp;
    bool curBtnDirDown = xboxController.xboxNotif.btnDirDown;

    // Button A: Trigger SIT Pose
    if (curBtnA && !prevBtnA) {
      Serial.println(F("[XBOX] Button A: SIT"));
      joyX = joyY = joyMag = 0.0f;
      poseSit();
    }

    // Button B: Trigger STAND Pose (slow raise if was sitting)
    if (curBtnB && !prevBtnB) {
      Serial.println(F("[XBOX] Button B: STAND"));
      joyX = joyY = joyMag = 0.0f;
      poseStand(motionState == ST_SIT);
    }

    // Button X: Trigger GREET Gesture Wave
    if (curBtnX && !prevBtnX) {
      Serial.println(F("[XBOX] Button X: GREET"));
      joyX = joyY = joyMag = 0.0f;
      doGreeting();
    }

    // D-Pad Up: Increase standing height level
    if (curBtnDirUp && !prevBtnDirUp) {
      if (hLevel < 4) {
        hLevel++;
        Serial.print(F("[XBOX] D-Pad Up: HEIGHT+ level=")); Serial.println(hLevel);
        applyHeight();
      }
    }

    // D-Pad Down: Decrease standing height level
    if (curBtnDirDown && !prevBtnDirDown) {
      if (hLevel > 0) {
        hLevel--;
        Serial.print(F("[XBOX] D-Pad Down: HEIGHT- level=")); Serial.println(hLevel);
        applyHeight();
      }
    }

    // Update debouncer states
    prevBtnA = curBtnA;
    prevBtnB = curBtnB;
    prevBtnX = curBtnX;
    prevBtnDirUp = curBtnDirUp;
    prevBtnDirDown = curBtnDirDown;

  } else {
    if (xboxConnected) {
      Serial.println(F("[XBOX] Controller DISCONNECTED."));
      xboxConnected = false;
      
      // Safety: stop gait and stand still when connection is lost
      joyX = 0.0f;
      joyY = 0.0f;
      joyMag = 0.0f;
      if (motionState == ST_WALK) {
        gaitRunning = false;
        poseStand(true);
      }
    }
  }
}
