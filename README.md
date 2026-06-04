# REXA-SpotmicroESP32-XBOX-Robotdog<br><br>REXA is an advanced open-source quadruped robot dog based on the **Nova SM3 (SpotMicro)** frame variant. Powered by an **ESP32** microcontroller, REXA utilizes 12 high-torque MG996R servos coordinated via a PCA9685 PWM driver, stabilized by an MPU6050 IMU, and controlled wirelessly via WiFi and an XBOX controller interface.<br><br><br><br> 🚀 Features<br> Inverse Kinematics (IK) Gait Engine: Real-time 3-DOF leg trajectory calculation for smooth walking, turning, sitting, and standing.<br> Onboard Access Point: Broadcasts its own secure WiFi network (`REXA`) for quick configuration, calibration, and local telemetry.<br> Persistent Calibration: Stores custom, per-joint servo offsets and local network configurations directly to the ESP32 flash memory using `LittleFS` and JSON serialization.<br> Asynchronous Servo Interpolation: Smooth, time-parameterized servo transitions decoupled from the main 50Hz control loop to minimize jitter.<br><br><br><br>🛠️ Hardware Specifications<br><br> Main Electronics & Sensors<br> Microcontroller: ESP32 (38-pin nodeMCU or similar)<br> PWM Servo Driver: PCA9685 (16-Channel, 12-bit I2C)<br> IMU (Gyro/Accelerometer): MPU6050<br> Actuators: 12 x MG996R High-Torque Digital Servos<br> Power Delivery: 5V-6V High-Amperage External UBEC / Buck Regulator (Dedicated Servo Rail)<br><br>### Physical Dimensions (Nova SM3 Frame)<br><br>| Parameter | Specification |<br>| :--- | :--- |<br>| **Coxa Length** | 30.0 mm |<br>| **Femur Length** | 105.0 mm |<br>| **Tibia Length** | 130.0 mm |<br>| **Hip Width (L/R Center)** | 150.0 mm |<br>| **Hip Length (F/R Center)** | 245.0 mm |<br>| **Default Standing Height ($Z$)** | 205.0 mm |<br>| **Approximate Total Weight** | ~2.6 kg (with batteries) |<br><br>---<br><br>## 🔌 Pin Mapping & Wiring<br><br>### I2C Bus Connections<br>* `ESP32 Pin 21` ➡️ **SDA** (PCA9685 & MPU6050)<br>* `ESP32 Pin 22` ➡️ **SCL** (PCA9685 & MPU6050)<br><br>### PCA9685 Servo Channel Map<br>Each leg utilizes 3 consecutive channels arranged as `[Coxa, Femur, Tibia]`:<br>* **Front Right (RF):** Channels 0, 1, 2<br>* **Back Right (BR):** Channels 3, 4, 5<br>* **Back Left (BL):** Channels 6, 7, 8<br>* **Front Left (FL):** Channels 9, 10, 11<br><br>> ⚠️ **CRITICAL POWER WARNING:** Do not power the MG996R servos directly from the ESP32 5V pin or USB. Connect an external 5V-6V high-current power supply directly to the PCA9685 screw terminals to prevent controller brownouts.<br><br>---<br><br>## 📶 Quick Start & Connection<br><br>1. **Power up REXA.** Wait 10–15 seconds for the boot sequence to complete.<br>2. Open your device's WiFi settings and connect to the robot's Access Point:<br>   * **SSID:** `REXA`<br>   * **Password:** `#Amma7965@`<br>3. Open a web browser and navigate to `http://192.168.4.1` or `http://rexa.local` to access the configuration and joint calibration UI.<br><br>---<br><br>## 🏗️ Software Architecture & Project Structure<br>* `config.h` — Master system constants, physical dimensions, pin maps, and gait timings.<br>* `Calibration.h / .cpp` — LittleFS file structures for saving and loading JSON servo offset tables.<br>* `src/` — Inverse kinematics engine, state machine, and XBOX controller parsing logic.


## 🌐 Socials:
[![Instagram](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/reddix_robotics) 

# 💻 Tech Stack:
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=plastic&logo=c%2B%2B&logoColor=white) ![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=plastic&logo=Arduino&logoColor=white) ![JWT](https://img.shields.io/badge/JWT-black?style=plastic&logo=JSON%20web%20tokens)
# 📊 GitHub Stats:
![](https://github-readme-stats.shion.dev/api?username=reddixrobotics&theme=dracula&hide_border=false&include_all_commits=true&count_private=true)<br/>
![](https://streak-stats.demolab.com/?user=reddixrobotics&theme=dracula&hide_border=false)<br/>
![](https://github-readme-stats.shion.dev/api/top-langs/?username=reddixrobotics&theme=dracula&hide_border=false&include_all_commits=true&count_private=true&layout=compact)

## 🏆 GitHub Trophies
![](https://github-profile-trophy.vercel.app/?username=reddixrobotics&theme=merko&no-frame=false&no-bg=false&margin-w=4)

### ✍️ Random Dev Quote
![](https://quotes-github-readme.vercel.app/api?type=horizontal&theme=dark)

---
[![](https://komarev.com/ghpvc/?username=reddixrobotics&icon=3&color=4)](https://visitcount.itsvg.in)

<!-- Proudly created with GPRM ( https://gprm.itsvg.in ) -->

