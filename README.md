# 📸 Smarter Doorbell: Edge AI on ESP32-S3

> **ECE635 Course Project** | **Status:** Active Development

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)](https://www.espressif.com/)
[![TinyML](https://img.shields.io/badge/TinyML-Edge%20Impulse-orange)](https://www.edgeimpulse.com/)
[![Home Assistant](https://img.shields.io/badge/Integration-Home%20Assistant-blue)](https://www.home-assistant.io/)

A privacy-centric, ultra-low-power smart doorbell solution powered by Edge Computing and TinyML. Runs entirely on the **Seeed Studio XIAO ESP32-S3 Sense** for under $20 USD.

---

## 📂 Repository Structure (Deliverables)

```text
/repo_root/
├── ESP32S3Code/
│   ├── sleepand wakeup.ino                                               # PIR Wake-up Sample Code
│   └── classification.ino                                                # Facial Recognition Example Code
│   └── ultralowpowerclassification.ino                                   #Full Integrated Project Code
│   └── lib/ei-smart-doorbell(new)--facial-recognition-arduino-1.0.6.zip  #TinyML Library Dependencies
├── Model_training/
│   ├── ei-smart-doorbell(new)--facial-recognition-arduino-1.0.8          # Quantized Model (typically Int8 Model)
│   └── ei-smart-doorbell(new)--facial-recognition-arduino-1.0.9          # Unquantized Model (typically Float32 Model)
│   └── smart-doorbell(new)-facial-recognition-export.zip                 # (Optional) Sample datasets
├── Doc/
│   ├── Course Report.pdf                                                 # Final Project Report
│   └── Presentation_Smarter Doorbell ESP32S3.pptx                        # Project Presentation PPT
├── 3D/
│   ├── SB_TOP.stl                                                        # Top Enclosure
│   └── SB_Bottom.stl                                                     # Bottom Enclosure
└── README.md                                                             # Project documentation
```

## 📖 Introduction & Motivation

### The Problem
Traditional smart doorbells (Ring, Nest, etc.) rely heavily on the cloud, introducing **privacy risks**, **high latency**, and **subscription costs**. Data is constantly streamed to remote servers, and battery life often suffers from inefficient detection methods.

### The Solution
This project implements a **local facial recognition system** on a resource-constrained microcontroller. By integrating a PIR sensor for wake-up triggers and utilizing deep-sleep modes, the device achieves extended battery life while ensuring **no video data ever leaves your local network**.

### Key Features
* **🔐 Privacy First:** 100% on-device processing.
* **🧠 TinyML Powered:** Uses **MobileNetV2** (Int8 quantized) for real-time facial recognition via Edge Impulse.
* **🔋 Ultra-Low Power:** ~83 µA Deep Sleep / ~380 mA Active. Estimated **55 days** battery life (400mAh, 10 triggers/day).
* **⚡ Fast Wake-up:** Hardware interrupt via PIR sensor triggers system wake-up.
* **🏠 Smart Home Ready:** Seamless integration with **Home Assistant** via MQTT.

---

## 🏗️ System Architecture

### Hardware Block Diagram
<img src="https://github.com/Zachay-NAU/ECE635/blob/main/Doc/ECE635_1.png" width="700" alt="System Block Diagram">

### The Pipeline
1.  **Idle:** System stays in Deep Sleep to conserve power.
2.  **Trigger:** PIR Sensor detects motion -> External Interrupt (GPIO) wakes the ESP32-S3.
3.  **Capture:** Camera initializes and captures a snapshot.
4.  **Inference:** Edge Impulse model runs locally (Face Detection & Recognition).
5.  **Action:**
    * If **Known Face**: Publish identity and confidence score to MQTT.
    * If **Unknown**: Publish security alert to MQTT.
6.  **Sleep:** System disconnects WiFi and returns to Deep Sleep (Total cycle < 5s).

---

## 🛠️ Hardware & Tech Stack

### Bill of Materials (BOM)
| Component | Description | Estimated Cost |
| :--- | :--- | :--- |
| **MCU** | [Seeed Studio XIAO ESP32-S3 Sense](https://www.seeedstudio.com/xiao-esp32-s3-sense-p-5639.html) (w/ OV2640 Cam) | ~$14.00 |
| **Sensor** | Grove - Digital PIR Motion Sensor | ~$5.00 |
| **Battery** | 3.7V Li-Po Battery (400mAh or larger recommended) | ~$5.00 |
| **Housing** | Custom 3D Printed PLA Enclosure | ~$1.00 (Filament) |

### Technology Stack
* **Firmware Framework:** Arduino (C++) / PlatformIO
* **Machine Learning:** [Edge Impulse](https://www.edgeimpulse.com/) (MobileNetV2 SSD)
* **Communication:** MQTT Protocol (PubSubClient)
* **Backend/Integration:** Home Assistant (Mosquitto Broker)

---

## 🚀 Getting Started

### 1. Prerequisites
* **IDE:** Install [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/).
* **Board Support:** Add `ESP32` by Espressif Systems to your Board Manager.
* **Libraries:**
    * `Edge Impulse Library` (Exported from your Edge Impulse project).
    * `PubSubClient` (by Nick O'Leary) for MQTT.

### 2. Wiring Diagram
| PIR Sensor Pin | XIAO ESP32-S3 Pin | Function |
| :--- | :--- | :--- |
| **VCC** | 3.3V or 5V | Power |
| **GND** | GND | Ground |
| **SIG** | D1 (GPIO 1) | Wake-up Interrupt Source |

### 3. Configuration
Rename `secrets_example.h` to `secrets.h` in your `src` folder and update your network credentials:

```cpp
// secrets.h
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "Your_WiFi_Name"
#define WIFI_PASS "Your_WiFi_Password"

#define MQTT_SERVER "192.168.1.X" // IP address of your Home Assistant/Broker
#define MQTT_PORT 1883
#define MQTT_USER "mqtt_user"     // Optional
#define MQTT_PASS "mqtt_password" // Optional

#endif
```
### 4. Deploy Model
Train your model on Edge Impulse (Target: ESP32-S3).

Go to Deployment -> Arduino Library.

Download the .zip library.

In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library....

Flash the firmware to the ESP32-S3.

