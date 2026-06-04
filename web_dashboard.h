#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include "config.h"
#include "gait.h"
#include "sensors.h"
#include "xbox_control.h"
#include "calibration.h"

// Web Server instance on port 80
extern WebServer server;

// Initialize WiFi AP and setup HTTP web server routes
void initWebDashboard();

// Handle incoming HTTP client requests (call in main loop and wait loops)
void handleWebRequests();

// Hook to handle web requests and BLE Xbox inputs simultaneously in blocking loops
void handleNetworkAndBluetooth();
