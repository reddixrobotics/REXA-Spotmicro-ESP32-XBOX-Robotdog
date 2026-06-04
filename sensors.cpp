#include "sensors.h"
#include <Wire.h>

float bodyPitch = 0.0f;
float bodyRoll = 0.0f;
bool imuReady = false;

static unsigned long lastIMURead = 0;

void initSensors() {
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Power management: wake up MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0x00); // Set to 0 to wake it up
  if (Wire.endTransmission() == 0) {
    Serial.println(F("[IMU] MPU6050 detected and initialized."));
    imuReady = true;
    lastIMURead = millis();
  } else {
    Serial.println(F("[IMU] MPU6050 connection failed! Stabilization disabled."));
    imuReady = false;
  }
}

void updateSensors() {
  if (!imuReady) return;

  unsigned long now = millis();
  float dt = (now - lastIMURead) / 1000.0f;
  if (dt <= 0.0f) return;
  lastIMURead = now;

  // Read Accelerometer and Gyroscope raw bytes in one block (14 registers: 0x3B - 0x48)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Starting register for Accel X
  if (Wire.endTransmission(false) != 0) {
    return; // I2C communication error
  }
  
  Wire.requestFrom((uint8_t)MPU_ADDR, (size_t)14, (bool)true);
  if (Wire.available() < 14) return;

  // Accel X, Y, Z
  int16_t ax = (Wire.read() << 8) | Wire.read();
  int16_t ay = (Wire.read() << 8) | Wire.read();
  int16_t az = (Wire.read() << 8) | Wire.read();
  // Temperature (skip)
  Wire.read(); Wire.read();
  // Gyro X, Y, Z
  int16_t gx = (Wire.read() << 8) | Wire.read();
  int16_t gy = (Wire.read() << 8) | Wire.read();
  int16_t gz = (Wire.read() << 8) | Wire.read();

  // 1. Calculate static Pitch and Roll from Accelerometer gravity vectors
  // Pitch is rotation about Y-axis (nose up/down)
  float accelPitch = atan2f((float)ay, sqrtf((float)ax * ax + (float)az * az)) * (180.0f / M_PI);
  // Roll is rotation about X-axis (side roll)
  float accelRoll = atan2f(-(float)ax, (float)az) * (180.0f / M_PI);

  // 2. Convert raw Gyro rate to degrees/second (MPU6050 default range +/- 250 deg/sec = 131 LSB/deg/s)
  float gyroX_rate = (float)gx / 131.0f;
  float gyroY_rate = (float)gy / 131.0f;

  // 3. Complementary Filter (integrates high-speed gyro drift + steady state accelerometer gravity reference)
  bodyPitch = 0.96f * (bodyPitch + gyroY_rate * dt) + 0.04f * accelPitch;
  bodyRoll  = 0.96f * (bodyRoll  + gyroX_rate * dt) + 0.04f * accelRoll;
}

float getStabilizationOffsetZ(uint8_t leg) {
  if (!imuReady) return 0.0f;

  // Geometrically derived leveling coefficients based on actual chassis dimensions
  // dz = tan(angle_rad) * (dimension / 2)
  // For small angles, tan(angle) ~ angle * PI / 180 (approx 0.017453)
  const float K_P_PITCH = (HIP_LENGTH / 2.0f) * 0.017453f; // ~2.14mm per degree
  const float K_P_ROLL  = (HIP_WIDTH  / 2.0f) * 0.017453f; // ~1.31mm per degree

  float pitchOffset = K_P_PITCH * bodyPitch;
  float rollOffset  = K_P_ROLL  * bodyRoll;

  float dz = 0.0f;

  switch (leg) {
    case RF: // Front Right: extends if head is down or right side is down
      dz = pitchOffset + rollOffset;
      break;
    case FL: // Front Left: extends if head is down or left side is down
      dz = pitchOffset - rollOffset;
      break;
    case BR: // Back Right: extends if tail is down or right side is down
      dz = -pitchOffset + rollOffset;
      break;
    case BL: // Back Left: extends if tail is down or left side is down
      dz = -pitchOffset - rollOffset;
      break;
  }

  // Cap stabilization to prevent servo over-travel (max +/- 22mm)
  return constrain(dz, -22.0f, 22.0f);
}
