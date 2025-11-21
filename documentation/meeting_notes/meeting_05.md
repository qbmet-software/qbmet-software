# 🗓️ Weekly Project Meeting Notes

**Project:** QBMeT Software  
**Date:** 2025-11-13 
**Time:** 6pm  
**Location/Platform:** BMH Rm121  
**Facilitator:** Nathan Duncan  
**Note-taker:** Nathan Duncan 

---

## 🧭 Agenda
1. Introductions
2. Work
3. Next Steps

---

## 1. Ice Breaker
What is the weirdest thing you've ever eaten?

---
## 2. Work on Projects
Today we worked on two of our projects. Colin and Alex were cooking on the digital twin
### Digital Twin Project
Validate state estimation by creating a digital twin of the lower body exoskeleton. Create a double pendulum model per set of legs and have the pendulum angles be interpreted from MPU readings.  
(Can you do two MPUs?)  


### Rerun Project
Assuming all hardware issues have been fixed. Try running last years suit code on the actual suit. Note the code on the repo we all are on only targets one motor currently. Make note of any bugs or hardware failures you find.
Will it work?

---
## 3. Project Assignment
### Project Management & Leadership

| Role | Member | Primary Responsibility |
| :--- | :--- | :--- |
| **Project Manager** | Nathan D. | Overall project delivery, timeline management, cross-team integration (software/mechanical), and risk mitigation. |
| **Team Lead: Sensors** | **Victor** | Technical direction, code review, and integration for all sensor/data acquisition systems. |
| **Team Lead: Control** | **Mohammed** | Technical direction, code review, and integration for all actuation/control systems. |

---

### 2. 📡 Sensors & Simulation Sub-Team (5 Members)

**Team Lead:** **Victor**  
**Objective:** Accurately acquire and process all user and environmental data, and provide robust, validated data models for the control system.

| Member | Project(s) |
| :--- | :--- |
| **Rena** | Digital Twin Project |
| **Colin** | Digital Twin Project |
| **Alex** | Rerun Project, Sensor Expansion Project |
| **Nathan** | Rerun Project, Sensor Expansion Project |
| **Zephra** | Rerun Project, Sensor Expansion Project |
| **Aahash** |Digital Twin Project |
| **Francesca** | PID Project |
| **Ian** | PID Project |
| **Siera** | PID Project |

---

## 3. 📅 Next Steps
- [ ] **Control Team**  A good first project could be designing/testing a simple PID for user input/hardcoded set points. Just go off motor encoder information no need to use the MPU.

- [ ] **Modeling** () Try to make a double pendulum model of a double leg. I would imagine a complementry filter could be useful for this.

- [ ] **Sensor Integration** Understand the hardware we have for sensors, (what are the pins on the MPU, what is the MUX for, how does it encode data, how do we interpret it?) 

- [ ] **PM and CTO stuff** Document the code base, what were the short commings (of code) last year? (Still none to my knowledge) Make a master overview presentation.

- **Next Meeting:** Play with the new single link system. Start wireless project. Everyone else work on their projects.

---

