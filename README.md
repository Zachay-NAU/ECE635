# ECE635-Smart Doorbell using ESP32S3/Raspberry Pi and tinyML

## Introduction
This project focuses on building a Smart Doorbell that leverages edge computing and tinyML principles. The core goal is to create a secure and private home application that can identify visitors locally on a low-power, resource-constrained device like a Raspberry Pi or ESP32-S3 with a camera.
## Goal
Use tinyML to design a Smart Doorbell system with utral tiny model and lowest power consumption.
## Deliverables:
### 1. FUNCTIONAL_EDGE_SYSTEM (Core Product)
# Core deliverable is the operational smart doorbell unit.
README.md: "The Raspberry Pi/ESP32-S3 will run an optimized TFLite model to perform low-latency face/person classification, distinguishing between known and unknown visitors."

### 2. CODE_AND_DOCUMENTATION
All source code, training assets, and guides.
/src/
    main.py: "Edge deployment code to run system logic and perform TFLite inference."
/model_training/
    train_model.ipynb: "Training scripts used for data preparation, model training, and conversion to TFLite format."
/docs/
    SETUP_GUIDE.md: "Detailed instructions on hardware wiring, OS configuration, and software environment setup."
    MODEL_OPTIMIZATION.md: "Guide detailing quantization and optimization steps for efficient edge deployment."
/repo_root/
    .gitattributes: "A complete GitHub repository will be provided, containing the model training scripts, edge deployment code, and detailed setup/operation guides."

### 3. PERFORMANCE_DEMONSTRATION
Verification and proof of concept.
/demo/
    demo_video.mp4: "A video demonstration will be delivered, showcasing the system's real-time inference on the edge device."
    PERFORMANCE_LOG.txt: "Recorded and reported data on the model's performance, including inference time (latency) and memory usage."

### 4. OPTIONAL_FEATURES
Advanced features, if implemented.
/src/notification_module.py: "Code enabling the system to trigger mobile notifications based on classification results."
/model_training/delivery_person_dataset/: "If implemented, the system will be able to trigger mobile notifications based

## Motivation
1. Enhanced Functionality and Security
Smarter Detection: Go beyond simply detecting motion or a button press. By using tinyML (Machine Learning on small devices), the doorbell can perform real-time object recognition (e.g., detecting a person vs. a car vs. a falling leaf) or facial recognition right at the door.

Prevent False Alarms: Advanced detection reduces annoying notifications caused by pets or shadows, only alerting the user to truly relevant events.

2. Overcoming Traditional Smart Doorbell Limitations
Speed (Low Latency): Traditional smart doorbells send video to the cloud for analysis, which causes a delay. Running the AI model locally on the ESP32S3 (at the "edge") allows for instantaneous analysis and alerts—a critical feature for security.

Privacy: Processing images and video on the device means sensitive data isn't constantly streamed to and stored on a company's cloud server, significantly improving user privacy.

3. Leveraging Edge Technology
Low Power Consumption: tinyML models are extremely efficient. This makes the project ideal for a battery-powered device like a doorbell, allowing it to be "always-on" while minimizing battery drain—especially important for the power-constrained ESP32S3.

Cost-Effectiveness: Building the solution with low-cost, widely available hardware like the ESP32S3 or Raspberry Pi allows for a powerful, customized smart home solution at a much lower cost than commercial high-end smart doorbells.
## System Blocks

<img src="https://github.com/Zachay-NAU/ECE635/blob/main/ECE635_1.png" width="700">

## hw/sw requirements

Hardware/Software: Raspberry Pi, camera module. For training, Google Colab can be used. You
can use any model for face detection. There are various approaches. One simple way is to compute
embeddings of known/trusted people using a CNN and store them. When a person approaches the
door, compare their embedding with known embeddings. If the embeddings match, you know it is a
trusted person.
## Team members responsibilities

### QUAN

---

## Phase 1: 🛠️ Hardware & Environment Setup

| ID | Task Description | Platform Focus | Status |
| :--- | :--- | :--- | :--- |
| **1.1** | **Finalize Platform Selection:** Commit to either **ESP32S3** (low-power, simple) or **Raspberry Pi** (high-power, complex features). | Hardware | ⬜ To Do |
| **1.2** | **Procurement:** Order main board, **Camera Module** (e.g., OV2640/RPi Camera), push button, microphone, and power components/battery. | Procurement | ⬜ To Do |
| **1.3** | **Base OS/Firmware Setup:** Install the necessary OS (RPi) or flash the base **Wi-Fi-enabled firmware** (ESP32S3). | Software/OS | ⬜ To Do |
| **1.4** | **Initial Hardware Test:** Verify that the camera, button, and basic network connectivity are functional. | Hardware | ⬜ To Do |

---

## Phase 2: 🧠 tinyML Model Development

| ID | Task Description | Tools/Methods | Status |
| :--- | :--- | :--- | :--- |
| **2.1** | **Data Collection:** Collect and label a diverse dataset of images/video of target objects (**Person, Package, Animal**) and negative examples (**Background**). | Camera Stream, Mobile App | ⬜ To Do |
| **2.2** | **Feature Engineering:** Preprocess data (e.g., image resizing, color space conversion) and prepare features using an ML platform (e.g., **Edge Impulse**). | Cloud/Local Tools | ⬜ To Do |
| **2.3** | **Model Training:** Design and train a lightweight visual model (e.g., a small CNN or **FOMO** for object detection). | TensorFlow Lite, Keras | ⬜ To Do |
| **2.4** | **Optimization & Quantization:** Apply **8-bit quantization** to the model to reduce its size and computational requirements for the target hardware. | TensorFlow Lite Micro | ⬜ To Do |
| **2.5** | **Model Testing:** Evaluate the optimized model's **accuracy and latency** using a validation dataset. | Testing Tools | ⬜ To Do |

---

## Phase 3: 💻 Edge Deployment & Integration

| ID | Task Description | Integration Point | Status |
| :--- | :--- | :--- | :--- |
| **3.1** | **Model Deployment:** Export the tinyML model as a **C/C++ library** and integrate it into the microcontroller firmware/application. | TFLite Micro | ⬜ To Do |
| **3.2** | **Inference Integration:** Write code to capture an image from the camera and feed it directly to the local ML model for **real-time inference**. | Camera/Model API | ⬜ To Do |
| **3.3** | **Event Trigger Logic:** Implement the logic to wake the device (from deep sleep for ESP32S3) upon button press or motion sensor activation, triggering the inference step (3.2). | GPIO/Interrupts | ⬜ To Do |
| **3.4** | **Notification Service:** Implement the network communication (e.g., **MQTT, Webhook, Telegram API**) to send an alert (with the snapshot) only when the model confirms a valid detection (e.g., "Person Detected"). | Wi-Fi/Cloud API | ⬜ To Do |

---

## Phase 4: ✅ Testing & Finalization

| ID | Task Description | Goal/Metric | Status |
| :--- | :--- | :--- | :--- |
| **4.1** | **Functional Testing:** Verify that all components (button, camera, inference, notification) work together reliably in a full cycle. | **Zero Fatal Errors** | ⬜ To Do |
| **4.2** | **Performance Check:** Measure the total inference time and the rate of **False Positives** (should be minimized). | Latency & Accuracy | ⬜ To Do |
| **4.3** | **Power Consumption Check:** (ESP32S3 only) Measure the deep sleep current and active current to estimate **battery life**. | Low-Power Target | ⬜ To Do |
| **4.4** | **Enclosure & Installation:** Design/3D print a weather-resistant casing and mount the device at the desired location. | Durability | ⬜ To Do |

---

### QUAN'S CAT--SUN

Cause Troubles
![bf2069c4019aa771a5fabd3feed1d9ab](https://github.com/user-attachments/assets/c301493e-86f7-4b8f-b4a1-f3cb5c51f381)



## Project timeline

# 🏠 Home Safety System Timeline

### 📅 October 1st - December 7th

---

### **Week 1: Oct 1 - Oct 7**
**Goal: Project Setup & Research**
- 🔍 Research LLMs and IoT devices.
- 📋 Define project requirements and system functionalities.
- 🛠 Set up development environment and tools.

---

### **Week 2: Oct 8 - Oct 14**
**Goal: System Architecture & Initial Prototypes**
- 🧠 Design system architecture (LLM, sensors, backend).
- 🔗 Prototype IoT sensor connectivity and data streaming.
- 🛠 Begin initial backend setup for data handling.

---

### **Week 3: Oct 15 - Oct 21**
**Goal: Frontend & Backend Development**
- 🛠 Develop backend for managing sensor data and alerts.
- 💻 Build basic user interface for real-time data monitoring.
- 🧠 Integrate LLM with sensor data for basic risk detection.

---

### **Week 4: Oct 22 - Oct 28**
**Goal: LLM Fine-Tuning & IoT Device Integration**
- 🤖 Fine-tune LLM for accurate threat detection.
- 🚨 Implement basic alert notifications (SMS, email).
- 🔗 Finalize IoT device integration with backend.

---

### **Week 5: Oct 29 - Nov 4**
**Goal: System Testing & Debugging**
- 🧪 Test system performance and troubleshoot issues.
- 🔧 Debug IoT device connections and data reliability.
- 🚨 Refine alert mechanisms and user interface.

---

### **Week 6: Nov 5 - Nov 11**
**Goal: Advanced Features & Personalization**
- 🎛 Add user customization for alerts and notifications.
- 🔄 Implement feedback loops for continuous learning.
- 💻 Improve user interface with additional features.

---

### **Week 7: Nov 12 - Nov 18**
**Goal: External Data Integration & Optimization**
- 🌍 Integrate external data sources (e.g., weather, fire alerts).
- ⚡ Optimize system for faster response times and better detection.
- 🛠 Refine front-end design based on feedback.

---

### **Week 8: Nov 19 - Nov 25**
**Goal: Final Testing & Documentation**
- 🧪 Conduct final tests of the entire system.
- 📚 Complete system documentation (setup, usage guide).
- 🚨 Ensure alerts work across all communication channels.

---

### **Week 9: Nov 26 - Dec 2**
**Goal: Deployment & User Testing**
- 🚀 Deploy system for initial user testing in a real environment.
- 💬 Collect feedback from users on performance and experience.
- 🔧 Make adjustments based on feedback.

---

### **Week 10: Dec 3 - Dec 7**
**Goal: Final Adjustments & Launch**
- 🔧 Apply final optimizations and tweaks.
- 🚀 Official system launch with full features.

---

### ✅ **Final Milestone: December 7th**  
Complete and functional Home Safety System ready for use! 🎉

