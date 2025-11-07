# 🗓️ Weekly Project Meeting Notes

**Project:** QBMeT Software  
**Date:** 2025-10-30  
**Time:** 6pm  
**Location/Platform:** BMH Rm121  
**Facilitator:** Nathan Duncan  
**Note-taker:** Nathan Duncan 

---

## 🧭 Agenda
1. Introductions
2. Team Breakdown
3. Complete Tutorial
4. Next Steps

---

## 1. Ice Breaker
Halloween Costumes!

---
## 2. Team Breakdown

# ⚙️ Software Sub-Team Structure: Lower-Body Exoskeleton

### 1. Project Management & Leadership

| Role | Member | Primary Responsibility |
| :--- | :--- | :--- |
| **Project Manager** | Nathan D. | Overall project delivery, timeline management, cross-team integration (software/mechanical), and risk mitigation. |
| **Team Lead: Sensors** | **Victor** | Technical direction, code review, and integration for all sensor/data acquisition systems. |
| **Team Lead: Control** | **Mohammed** | Technical direction, code review, and integration for all actuation/control systems. |

---

### 2. 📡 Sensors & Simulation Sub-Team (5 Members)

**Team Lead:** **Victor**  
**Objective:** Accurately acquire and process all user and environmental data, and provide robust, validated data models for the control system.

| Role | Member(s) | Focus Area(s) | Core Deliverables |
| :--- | :--- | :--- | :--- |
| **Modelers** | **Rena**, **Colin** | Simulation and Validation, Digital twin. | Development and expansion of the `visualize_body_angle` code and data validation models. |
| **Sensor Data Engineer** | **Alex**, **Nathan** | Data Acquisition and Filtering, Sensor Interfacing. | Robust data streams from existing sensors; managing `mpu_data_collection_mux` and ensuring data quality. |
| **Pressure Sensing Specialist** | **Zephra** | New Sensor Integration (Weight/Load Calculation). | Integrate and calibrate foot pressure sensors to calculate the total user load for the ACE competition objective. |
| **Connectivity & Edge Compute** | **Aahash** | Wireless Communication, Data Logging. | Utilizing **ESP32** for external communication (Wi-Fi/Bluetooth) and on-board data logging/debugging. |

---

### 3. 🔩 Actuation & Control Sub-Team (5 Members)

**Team Lead:** **Mohammed**  
**Objective:** Implement a safe, responsive, and efficient control scheme that translates high-level desired motion/assistance into precise motor commands.

| Role | Member(s) | Focus Area(s) | Core Deliverables |
| :--- | :--- | :--- | :--- |
| **Control System Engineer** | **Francesca**, **Ian** | Primary Motor Control Loop (PID), Tuning. | Design, implementation, and tuning of the primary `motor_control` logic for torque/angle precision. |
| **Advanced Control Researcher** | **Siera** | Algorithm Exploration (Beyond PID). | Research and prototype advanced control strategies (e.g., MPC, LQR) to optimize assistance and efficiency. |

---

## 3. MPU Tutorial
- **Summary:**  
  Follow the [ESP32 and MPU6050 Tutorial](https://www.instructables.com/How-to-Connect-MPU6050-to-ESP32-Physical-Setup-and/).

Some issues with installing libraries, others with running the code.
Note: Select the ESP32 Dev Module for the board
 - Double check the wiring diagram in the tutorial

---

## 4. 📅 Next Steps
- [ ] **Control Team** A good first project could be designing/testing a simple PID for user input/hardcoded set points. Just go off motor encoder information no need to use the MPU.

- [ ] **Modeling** Try to make a single or double pendulum model of a single or double leg. I would imagine a complementry filter could be useful for this.

- [ ] **Sensor Integration** Understand the hardware we have for sensors, (what are the pins on the MPU, what is the MUX for, how does it encode data, how do we interpret it?) 

- **Next Meeting:** We can get one or two people from each sub team to try running last years code on the exoskeleton. Everyone else work on their projects.

---

