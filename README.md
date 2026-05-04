# 🛢️ Automated Pipeline Leak Detection & Isolation System

## 📌 Project Overview
This repository documents the ongoing development of a miniature automated process safety system. Designed to simulate Safety Instrumented Systems (SIS) used in the Oil & Gas industry, this hardware prototype detects fluid loss in a pipeline network and autonomously isolates the compromised section to prevent spillage and environmental hazards.

## ⚙️ Core Engineering Logic: Mass Balance Principle
The system relies on continuous flow monitoring using the **Mass Balance Principle**:
* **Flow Sensor A** monitors the input flow rate.
* **Flow Sensor B** monitors the output flow rate.
* **The Logic Solver (ESP32)** continuously compares the two values: `ΔFlow = Rate A - Rate B`
* If `ΔFlow` exceeds the safe operational threshold (indicating a leak between the sensors), the microcontroller triggers an emergency shutdown (ESD) by closing a motorized solenoid valve and cutting power to the pump.

## 🛠️ Hardware Components
* **Logic Solver / Controller:** ESP32 Microcontroller (chosen for future IIoT dashboard integration)
* **Sensors:** 2x YF-S201 Hall Effect Water Flow Sensors
* **Final Control Elements:** 12V Motorized Solenoid Valve (Normally Open), 12V DC Water Pump
* **Switching:** 2-Channel 5V Relay Module

## 🚀 Development Roadmap (8-Month Build)
- [ ] **Phase 1:** Component acquisition, schematic diagramming (P&ID), and basic sensor calibration.
- [ ] **Phase 2:** Breadboard prototyping and C++ control logic development.
- [ ] **Phase 3:** Wet testing with PVC piping and simulated leak scenarios.
- [ ] **Phase 4:** SCADA/Cloud Dashboard integration using Web/IIoT tools and final physical casing.

## 👨‍💻 Author
**Ebubechukwu (Valentine) Amadi** | Electrical & Electronics Engineering (I&C) | UNILAG  
[Connect with me on LinkedIn](https://www.linkedin.com/in/ebube-ic)
