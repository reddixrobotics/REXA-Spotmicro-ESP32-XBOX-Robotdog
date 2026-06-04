#include "calibration.h"
#include <LittleFS.h>

CalibrationData calibration;

void CalibrationData::setDefaults() {
  memset(offsets, 0, sizeof(offsets));
  strncpy(wifiSSID, DEFAULT_WIFI_SSID, sizeof(wifiSSID));
  strncpy(wifiPass, DEFAULT_WIFI_PASS, sizeof(wifiPass));
}

bool loadCalibration() {
  calibration.setDefaults();

  if (!LittleFS.begin(true)) {
    Serial.println(F("[CALIB] LittleFS mount failed. Using defaults."));
    return false;
  }

  File file = LittleFS.open("/calibration.json", "r");
  if (!file) {
    Serial.println(F("[CALIB] No calibration file. Using defaults."));
    return false;
  }

  String content = file.readString();
  file.close();

  // Lightweight JSON parser – extracts offsets, ssid, pass
  int idx = content.indexOf("\"offsets\":");
  if (idx != -1) {
    int start = content.indexOf("[[", idx);
    if (start != -1) {
      int leg = 0, joint = 0;
      String num = "";
      for (unsigned int i = start + 2; i < content.length() && leg < 4; i++) {
        char c = content.charAt(i);
        if (isDigit(c) || c == '-') {
          num += c;
        } else if (c == ',' || c == ']') {
          if (num.length() > 0) {
            calibration.offsets[leg][joint] = num.toInt();
            num = "";
            joint++;
            if (joint >= 3) {
              joint = 0;
              leg++;
            }
          }
        }
      }
    }
  }

  int ssidIdx = content.indexOf("\"ssid\":\"");
  if (ssidIdx != -1) {
    int start = ssidIdx + 8;
    int end = content.indexOf('"', start);
    if (end != -1) {
      strncpy(calibration.wifiSSID, content.substring(start, end).c_str(), sizeof(calibration.wifiSSID));
    }
  }

  int passIdx = content.indexOf("\"pass\":\"");
  if (passIdx != -1) {
    int start = passIdx + 8;
    int end = content.indexOf('"', start);
    if (end != -1) {
      strncpy(calibration.wifiPass, content.substring(start, end).c_str(), sizeof(calibration.wifiPass));
    }
  }

  Serial.println(F("[CALIB] Loaded from LittleFS."));

  // Automatically migrate from old WiFi AP names to the new house WiFi
  if (strcmp(calibration.wifiSSID, "RobotDog") == 0 || strcmp(calibration.wifiSSID, "Robot_AP") == 0) {
    Serial.println(F("[CALIB] Migrating old SSID to DEFAULT_WIFI_SSID..."));
    strncpy(calibration.wifiSSID, DEFAULT_WIFI_SSID, sizeof(calibration.wifiSSID));
    strncpy(calibration.wifiPass, DEFAULT_WIFI_PASS, sizeof(calibration.wifiPass));
    saveCalibration();
  }

  return true;
}

bool saveCalibration() {
  if (!LittleFS.begin(true)) {
    Serial.println(F("[CALIB] LittleFS mount failed for save."));
    return false;
  }

  File file = LittleFS.open("/calibration.json", "w");
  if (!file) {
    Serial.println(F("[CALIB] Cannot open file for writing."));
    return false;
  }

  String json = "{\"offsets\":[";
  for (int i = 0; i < 4; i++) {
    json += "[";
    for (int j = 0; j < 3; j++) {
      json += String(calibration.offsets[i][j]);
      if (j < 2) json += ",";
    }
    json += "]";
    if (i < 3) json += ",";
  }
  json += "],\"ssid\":\"" + String(calibration.wifiSSID);
  json += "\",\"pass\":\"" + String(calibration.wifiPass) + "\"}";

  file.print(json);
  file.close();
  Serial.println(F("[CALIB] Saved to LittleFS."));
  return true;
}
