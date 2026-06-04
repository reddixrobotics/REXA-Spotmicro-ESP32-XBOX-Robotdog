#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "calibration.h"
#include "servo_driver.h"
#include "kinematics.h"
#include "gait.h"
#include "sensors.h"
#include "xbox_control.h"
#include "web_dashboard.h"

static unsigned long lastLoopTime = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(F("\n=============================================="));
  Serial.println(F("          REXA QUADRUPED FIRMWARE            "));
  Serial.println(F("=============================================="));

  // 1. Load Calibration Parameters from Flash Storage
  loadCalibration();

  // 2. Initialize Hardware Drivers
  initSensors();     // MPU6050 I2C
  initServos();      // PCA9685 I2C

  // 3. Immediately lock servos to safe default Standing Pose
  Serial.println(F("[BOOT] Moving servos to Standing Pose..."));
  poseStand(false);

  // 4. Initialize Wireless Communications
  initXbox();         // BLE Xbox gamepad host
  initWebDashboard(); // WiFi AP and Web Interface
  
  Serial.println(F("=============================================="));
  Serial.println(F("           SYSTEM INITIALIZED & READY         "));
  Serial.println(F("=============================================="));
  lastLoopTime = millis();
}

void loop() {
  // 1. Handle non-blocking background communications (WiFi Clients & BLE Gamepad)
  handleNetworkAndBluetooth();

  // 2. Precise 50 Hz execution scheduler loop (20ms intervals)
  unsigned long now = millis();
  if (now - lastLoopTime >= LOOP_MS) {
    lastLoopTime = now;

    // A. Read IMU pitch/roll telemetry and filter accelerometer/gyro readings
    updateSensors();

    // B. Smoothly interpolate active servos to target positions
    updateServos();

    // C. Process gait state transitions if standing or walking
    if (motionState == ST_STAND || motionState == ST_WALK) {
      updateGaitFromJoystick();
      
      if (gaitRunning) {
        stepGait();
      }
    }
  }
}
