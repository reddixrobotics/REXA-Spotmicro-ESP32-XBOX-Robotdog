#pragma once
#include <Arduino.h>

/*
 * ============================================================
 *  REXA QUADRUPED – MASTER CONFIGURATION
 *  Hardware: ESP32 + PCA9685 + MPU6050 + MG996R Servos
 *  Frame:    Nova SM3 Quadruped
 * ============================================================
 */

// ── I2C Pins for ESP32 ─────────────────────────────────────
#define I2C_SDA      21
#define I2C_SCL      22
#define MPU_ADDR     0x68
#define PCA_ADDR     0x40

// ── WiFi STA Settings ───────────────────────────────────────
#define DEFAULT_WIFI_SSID   "REXA"
#define DEFAULT_WIFI_PASS   "#Amma7965@"
#define WIFI_HOSTNAME       "rexa"

// ── PCA9685 PWM Settings ───────────────────────────────────
#define SERVO_FREQ   50
#define SERVO_MIN    135      // PWM tick for 0°
#define SERVO_MAX    545      // PWM tick for 180°

// ── Leg / Joint Indices ────────────────────────────────────
#define RF 0   // Front Right
#define BR 1   // Back Right
#define BL 2   // Back Left
#define FL 3   // Front Left

#define CX 0   // Coxa (hip yaw)
#define FM 1   // Femur (upper leg)
#define TB 2   // Tibia (lower leg)

// ── PCA9685 Channel Map ────────────────────────────────────
// Each leg uses 3 consecutive channels: [Coxa, Femur, Tibia]
const uint8_t CH[4][3] = {
  { 3,  4,  5},   // RF (Front Right)
  { 9, 10, 11},   // BR (Back Right)
  { 6,  7,  8},   // BL (Back Left)
  { 0,  1,  2}    // FL (Front Left)
};

// Mirroring: Right-side servos have non-mirrored direction
const bool IS_RIGHT[4] = {true, true, false, false};

// ── Leg Geometry (mm) — Nova SM3 Frame ─────────────────────
#define COXA_LEN     30.0f
#define FEMUR_LEN    105.0f
#define TIBIA_LEN    130.0f
#define MAX_REACH    235.0f   // Physical hard limit
#define STAND_HEIGHT 205.0f   // Default standing Z

// ── Body Dimensions (mm) — Nova SM3 Frame ──────────────────
#define HIP_WIDTH    150.0f   // Distance between left/right hip axes
#define HIP_LENGTH   245.0f   // Distance between front/rear hip axes

// ── Servo Zero Angles (neutral/straight-leg positions per leg) ─
const int COXA_ZERO[4]  = {90, 90, 90, 90};   // RF, BR, BL, FL
const int FEMUR_ZERO[4] = {90, 90, 90, 90};   // RF, BR, BL, FL
const int TIBIA_ZERO[4] = {90, 90, 90, 90};   // RF, BR, BL, FL

// ── Gait Constants ─────────────────────────────────────────
#define LEAN_X       20.0f    // mm foot forward of hip at stand
#define STRIDE       22.0f    // Half-stride in sagittal plane (mm)
#define LIFT_H       38.0f    // Foot lift height (mm)
#define TURN_SWEEP   16.0f    // Coxa degrees swept per spot turn cycle
#define SPOT_THRESH  0.60f    // Joystick magnitude threshold for spot turn

// ── Async Servo Interpolation ──────────────────────────────
#define LOOP_MS      20       // 50 Hz main loop
#define DEADBAND     2        // Servo position deadband (degrees)
#define STEP_WALK    2.5f     // Degrees per tick during walk
#define STEP_SLOW    0.8f     // Degrees per tick for slow transitions
#define STEP_MED     1.5f     // Degrees per tick for medium speed

// ── Height Levels (Z in mm) ────────────────────────────────
// Index 2 = default standing height (205mm)
// Max 220mm to prevent femur going past horizontal
const int HEIGHT_Z[5] = {165, 180, 205, 215, 220};

// ── Motion States ──────────────────────────────────────────
enum MotionState {
  ST_STAND,
  ST_SIT,
  ST_WALK,
  ST_GREET,
  ST_GREET_RETURN
};
