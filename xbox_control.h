#pragma once
#include <Arduino.h>
#include "config.h"
#include "gait.h"

// Xbox controller connection status
extern bool xboxConnected;

// Initialize Xbox BLE controller
void initXbox();

// Poll Xbox controller state and map inputs to gait variables
void updateXbox();
