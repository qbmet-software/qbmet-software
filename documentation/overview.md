# Exoskeleton Suit Control: Software Architecture & Codebase Documentation

**Document Version:** 1.0
**Last Updated:** November 1, 2025

## 1. Executive Overview 

This document details the software architecture for the `suit_control` firmware, which runs on an ESP32 microcontroller. The system's primary function is to provide **assistive torque** to a lower-body exoskeleton, suplementing the user's angular velocity at the hip and knee joints.

The system operates by:
1.  Reading real-time motion data from multiple Inertial Measurement Units (IMUs).
2.  Calculating the angular velocity for each joint along its primary axis of rotation.
3.  Applying a Proportional-Derivative (PD) control algorithm to compute a corrective torque.
4.  Transmitting this torque command to the appropriate motor controller via the CAN bus.

This architecture is designed to be modular, allowing for independent development and testing of the motor control, CAN communication, and sensor data processing components.

---

## 2. System Architecture Diagram

The software follows a clear data flow path from sensor input to motor output.

**Data Flow:**
`MPU6050 Sensors` -> `I2C Multiplexer` -> `ESP32 (Main Logic)` -> `CAN Handler` -> `Motors`

1.  **Sensors:** Four MPU6050 IMUs measure 3D acceleration and angular velocity at the right/left hip and knee.
2.  **Multiplexer:** A PCA9548A I2C multiplexer allows the ESP32 to communicate with each MPU individually, as they share the same I2C address.
3.  **ESP32 (Main Application Logic):** The core firmware reads the selected sensor, processes the data, runs the control algorithm, and determines the necessary motor torque.
4.  **CAN Handler:** This abstraction layer manages all low-level communication on the CAN bus.
5.  **Motors:** The motors receive torque commands via the CAN bus and report back their status (position, velocity, etc.).

---

## 3. Core Component Deep Dive 

This section breaks down the key C++ classes and the main application file.

### 3.1. Main Application (`suit_control_V2.ino`)

This file contains the central control logic and orchestrates all other components.

* **Purpose:** To serve as the "brain" of the exoskeleton. It initializes all hardware and runs the main high-frequency control loop.
* **Key Responsibilities:**
    * **Initialization (`setup()`):**
        * Sets up serial communication for debugging.
        * Initializes the `Wire` library for I2C communication with the MPUs on specific GPIO pins (`I2C_SDA_PIN`, `I2C_SCL_PIN`).
        * Initializes the CAN bus via the `canHandler` object.
        * Starts and zeros the motors. **Note:** Currently, only `motor1` (Right Hip) is active; others are commented out.
        * Sequentially initializes all four MPU6050 sensors by cycling through the I2C multiplexer channels (0, 1, 4, 5).
    * **Main Control Loop (`loop()`):**
        * **Sensor Reading:** It cycles through the I2C multiplexer to read gyroscope data from each MPU sequentially.
        * **Vector Projection:** A `dotProduct()` function is used to project the 3D gyroscope vector onto a pre-calibrated 1D axis vector (`mpuVec1`, `mpuVec2`, etc.). This critical step isolates the angular velocity (`omega`) for the specific joint's axis of rotation.
        * **PD Control Algorithm:** For the active right hip motor, it calculates a target torque using a Proportional-Derivative controller.
            * **Proportional (P) Term:** `-Kp_HIP * omega0`. This provides the primary assistive force, proportional to the angular velocity.
            * **Derivative (D) Term:** `-Kd_HIP * filtered_domega0`. This acts as a damping force to smooth the motion and prevent oscillation. A low-pass filter (`alpha_d`) is applied to the derivative to reduce noise sensitivity.
        * **Command Execution:** The calculated torque is sent to the motor using `motor1.sendCommand()`. A small deadband (`abs(omega0) > 0.01`) prevents motor activation from sensor noise at rest.
        * **Safety/Watchdog:** A crude watchdog timer resets and re-zeros the motors every 15 seconds to prevent runaway states during testing. This should be replaced with a more robust safety protocol in production.

### 3.2. Motor Class (`Motor.h`, `Motor.cpp`)

This class is an essential abstraction layer that simplifies interaction with the smart motors.

* **Purpose:** To provide a high-level API for controlling a motor without needing to know the specifics of the underlying CAN bus communication protocol (T-Motor/MIT protocol).
* **Key Features:**
    * **Command Packing (`packCommand`):** This is the most critical function. It takes physically meaningful floating-point values (position, velocity, torque, gains) and converts them into the specific integer format required by the motor controller. It then **packs** these integers into the 8 bytes of a CAN message frame according to the motor's communication protocol.
    * **Command Unpacking (`unpackCommand`):** Does the reverse of packing. It parses incoming CAN messages from the motor to extract feedback data like measured position, velocity, and torque, converting the raw integers back into floats.
    * **State Management:** The class holds the motor's ID, its operational state (`isStopped`), and its last known output values (`pOut`, `vOut`, `iOut`).
    * **Interface:** Provides simple methods like `start()`, `stop()`, `reZero()`, and `sendCommand(...)`.
    * **Periodic Updates (`update`):** This method should be called in the main loop. It retrieves the latest feedback from the `CANHandler`  and periodically re-sends the last command to ensure the motor remains active.

### 3.3. CAN Bus Handler (`CANHandler.h`, `CANHandler.cpp`)

A lightweight wrapper around the `ACAN_ESP32` library to manage all CAN bus traffic.

* **Purpose:** To centralize CAN bus initialization and message handling, decoupling the main application logic from the hardware communication details.
* **Key Functionality:**
    * **Setup (`setupCAN`):** Configures and starts the ESP32's CAN controller with the desired bit rate (1 Mbps) and pin assignments (GPIO 22 for TX, 21 for RX).
    * **Message Reception (`update`):** This is the core polling function. It must be called frequently in `loop()`. It checks the CAN hardware for new incoming messages and, if found, stores them in a `latestFrame` array. It also timestamps the reception time for each message.
    * **Data Retrieval (`getLatestMessage`):** The `Motor` class uses this function to retrieve the most recent message corresponding to its own CAN ID from the handler's cache. This is an efficient "pull" model that prevents the `Motor` class from needing to interact with the CAN library directly.
    * **Status Checking (`isMessageOnline`):** Provides a simple way to check if a specific motor is still communicating by comparing the current time to the timestamp of its last received message.

### 3.4. Remote Debug Utility (`RemoteDebug.h`, `RemoteDebug.cpp`)

A minimal, helper class for formatted debugging output.

* **Purpose:** To provide a simple, globally accessible debugging interface that can be easily expanded or rerouted in the future.
* **How it Works:** It defines a global `Debug` object. The `debugI` and `debugW` macros are wrappers around its `printf` method , which formats a string and prints it to the `Serial` monitor. While named `RemoteDebug`, the current implementation is local to the serial port but could be adapted to send debug messages over Wi-Fi or Bluetooth.

---

## 4. Key Configuration & Constants

To modify the system's behavior, engineers should primarily look at the global constants defined at the top of `suit_control_V2.ino`.

* **Pin Definitions:**
    * `CAN_TX_PIN = 22`, `CAN_RX_PIN = 21`.
    * `I2C_SDA_PIN = 23`, `I2C_SCL_PIN = 25`.
* **Control Gains:**
    * `Kp_HIP = 7.0`: Proportional gain for the hip joint. Increasing this will make the assistive force stronger but may lead to instability.
    * `Kd_HIP = 0.01`: Derivative gain for the hip joint. Increasing this will add more damping and make the motion feel smoother.
* **MPU Calibration Vectors (`mpuVec1`-`mpuVec4`):**
    * These floating-point arrays are **critically important**. They are unit vectors determined experimentally to define the exact axis of rotation for each joint in the IMU's 3D coordinate frame. If an IMU is physically remounted or rotated, its corresponding vector **must be recalibrated**.

---

## 5. Areas for Improvement & Future Work

* **Activate All Motors:** The current code only controls `motor1`. The logic needs to be expanded to read from all sensors  and control all four motors simultaneously, likely requiring separate PD controllers and state variables for each joint. 
This was included in a past version but was modified when parts of the exoskeleton broke.
* **Robust Safety System:** The 15-second motor reset is a temporary solution. A proper safety system should be implemented, including error code checking from motors (`motor.getErrorCode()` ), current/torque limits, and a reliable emergency stop protocol.
* **Dynamic Loop Timing:** The `loop()` uses a fixed `delay(10)` at the end , and `dt` is recalculated based on `millis()`. This can introduce jitter. A more robust approach would be to use a hardware timer to ensure the control loop executes at a precise frequency (e.g., 100 Hz).
* **Refactor Sensor Reading:** Reading sensors sequentially in the main loop introduces latency. A more advanced implementation might use interrupts or a dedicated RTOS task for sensor data acquisition to ensure the most recent data is always available to the control loop.