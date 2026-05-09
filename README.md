# 🛢️ Automated Pipeline Leak & Vandalism Detection System

## 📌 Project Overview
This repository documents the ongoing development of an advanced, miniature automated process safety system. Designed to simulate Safety Instrumented Systems (SIS) used in the Oil & Gas industry, this hardware prototype detects both **fluid loss (leaks)** and **physical sabotage (vandalism)**. Upon detecting an anomaly, the system autonomously isolates the compromised pipeline section to prevent environmental hazards and transmits real-time telemetry to a remote SCADA dashboard.

## 📐 System Architecture (P&ID)
![ISA Standard P&ID](Pipeline-Leak-Detection.drawio.png)

*Fig 1: ISA-5.1 compliant Piping and Instrumentation Diagram detailing the dual-threat Emergency Shutdown (ESD) logic.*

## ⚙️ Core Engineering Logic

### 1. Hydraulic Monitoring: Mass Balance Principle
The system relies on continuous flow monitoring to detect internal leaks:
* **Flow Sensor A (FT-201A)**: Monitors the input flow rate.
* **Flow Sensor B (FT-201B)**: Monitors the output flow rate.
* **The Logic Solver (ESP32)**: Continuously calculates the delta using `ΔFlow = Rate A - Rate B`.
* **ESD Trigger**: If `ΔFlow` exceeds the safe operational threshold, the system triggers an emergency shutdown by closing a motorized solenoid valve (SV-101) and cutting power to the pump (P-101).

### 2. Structural Monitoring: Edge AI & Vibration Analysis (TinyML)
To detect physical tampering (e.g., oil theft via hacksaws), the system uses on-device Machine Learning (TinyML):
* **Sensing Layer**: A 3-Axis Accelerometer (YT-301) captures raw physical vibration data from the pipeline surface.
* **Data Processing**: The system ingests **375 raw vibration features** (static float arrays).
* **Signal Processing**: A Digital Signal Processing (DSP) block utilizes Fast Fourier Transform (FFT) to convert raw shaking into **399 processed mathematical features**.
* **Classification**: A Quantized (int8) Neural Network running locally on the ESP32 classifies the state into: `Idle_Normal`, `Vandalism_Hacksaw`, or `Vandalism_Hammer`.
* **Safety Protocol**: If vandalism is detected with **>60% probability**, the system immediately executes a hardware-latched ESD.

## 🧠 Data Engineering Case Study: Model Optimization
**🔗 [View the Full AI Model & Dataset on Edge Impulse](https://studio.edgeimpulse.com/public/986688/latest)**

During the AI training phase, the model initially struggled to differentiate between a hacksaw and a hammer impact. 

**The Engineering Problem**: Data confusion caused by sensor orientation. Recording the "hacksaw" motion near the edge of the testing surface introduced micro-vertical bounces. These tiny bounces on the Z-axis looked exactly like a hammer strike, causing overlapping data clusters.

**The Solution**: I enforced strict data collection protocols, purging "bouncing" data and re-recording smooth friction signatures. The resulting **399-dimensional feature clusters** separated perfectly.

![DSP Feature Explorer](dsp-feature-explorer.png)
*Fig 2: DSP mapping showing clean separation of physical states (Idle, Hacksaw, Hammer).*

* **Final Model Accuracy**: 100%.
* **On-Device Performance**: The model utilizes only **2 KB of Peak RAM** and has an inference time of **~13 ms**.

## 📡 Communications & IIoT Integration
Industrial systems must report to central command. This prototype features dual-channel communication:
* **Wi-Fi (Primary - Blynk IoT)**: The ESP32 streams live pipeline metrics (System Status V0, AI Confidence V1) to a cloud dashboard.
* **Remote Reset (V2)**: Authorized personnel can remotely re-arm the system after a latch via a secure Blynk virtual pin.
* **GSM (Failover/Alerts)**: A SIM800L module is integrated for SMS text alerts if the Wi-Fi network fails during an ESD event.

## 🛠️ Hardware Stack
* **Logic Solver**: ESP32 Dev Module (Dual-core, Wi-Fi enabled).
* **AI Sensor**: MPU6050 3-Axis Accelerometer.
* **Flow Sensors**: 2x YF-S201 Hall Effect Water Flow Sensors.
* **Final Control Elements**: 12V Motorized Solenoid Valve, 12V DC Water Pump.
* **Switching**: Active-High Relay Module (RY-100).

## 🚀 Development Roadmap
- [x] **Phase 1**: Component acquisition and P&ID drafting.
- [x] **Phase 2**: Cloud AI architecture and training the Vandalism Detection Neural Network (100% Accuracy).
- [x] **Phase 3**: Integration of Safety Latch logic and Blynk IoT Telemetry.
- [ ] **Phase 4**: Merging Mass Balance hydraulic control with the AI library.
- [ ] **Phase 5**: Failover testing with SIM800L GSM module.
- [ ] **Phase 6**: Wet testing with PVC piping and final physical deployment.

## 👨‍💻 Author
**Ebubechukwu (Valentine) Amadi** | Electrical & Electronics Engineering (I&C) | UNILAG  
[Connect with me on LinkedIn](https://www.linkedin.com/in/ebube-ic)
