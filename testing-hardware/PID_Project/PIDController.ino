/*
This program repurposes the original motor control code (suit_control_V2.ino) to implement a simple PID controller using encoder feedback instead of MPU sensors as request by Nathan.
The ESP32 communicates with the motor controller over CAN bus through the CAN Handler and Motor classes. The internal encoder of the motor provides feedback on its current angular position.

Goal: To move the motor to a sequence of hardcoded position set points, and hold each position for a short time using a PID loop that continuously corrects the error between the target and actual position.

The torque command is sent to the motor using motor1.sendCommand(), producing smooth and stable motion between set points.

This script is the foundation for the first PID project mentioned by Nathan:
- Does not use an MPU
- Encoder based feedback only
- Hardocoded set points

If you guys have any questions, please DM me directly on Discord and I will get back to you as soon as possible!
*/

#include <Arduino.h>
#include <Wire.h>
#include "CANHandler.h"
#include "Motor.h"
#include "RemoteDebug.h"

//CAN/RX pins
static const int CAN_TX_PIN = 22;  // D22 = TX
static const int CAN_RX_PIN = 21;  // D21 = RX

//Global CAN handler
CANHandler canHandler;

Motor motor1(0x01, canHandler, Debug);

// Adjust these if needed
const float Kp_POS = 10.0f; //proportional gain
const float Ki_POS = 0.5f; //integral gain
const float Kd_POS = 0.2f; //derivative gain

//Side noteL: I recommend researching and understanding what each component of PID does to understand the three constants declared above.

// Limiters
const float I_MAX = 3.0f;
const float TORQUE_MAX = 9.0f;

// Control timing (currently at 100 Hz)
const unsigned long CONTROL_PERIOD_MS = 10;
const float dt = CONTROL_PERIOD_MS / 1000.0f;

// Hardcoded position setpoints
float setpoints[] = { 0.0f, 0.5f, -0.5f, 0.0f };
const int NUM_SETPOINTS = sizeof(setpoints) / sizeof(setpoints[0]);
int currentSetpointIndex = 0;

// PID memory
float integral_error = 0.0f;
float prev_error = 0.0f;
unsigned long lastControlTime = 0;
unsigned long lastSetpointReachedTime = 0;

float getMotor1PositionRad() {
  //NOTE: Replace this with actual encoder reading from Motor class
  return 0.0f;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting PID position control...");

  Debug.begin("ESP32_Motor_Controller");

  //Setup
  canHandler.setupCAN(CAN_TX_PIN, CAN_RX_PIN);
  Serial.println("CAN initialized.");

  // Initialize motor
  motor1.start();
  motor1.reZero();
  Serial.println("Motor ready.");

  lastControlTime = millis();
  lastSetpointReachedTime = millis();
}

void loop() {
  canHandler.update();
  motor1.update();

  unsigned long now = millis();

  //Run control loop at fixed rate
  if (now - lastControlTime >= CONTROL_PERIOD_MS) {
    lastControlTime += CONTROL_PERIOD_MS;

    float pos = getMotor1PositionRad();
    float sp = setpoints[currentSetpointIndex];
    float error = sp - pos;  //if the motion reverses, flip sign here

    integral_error += error * dt;
    if (integral_error > I_MAX) integral_error = I_MAX;
    if (integral_error < -I_MAX) integral_error = -I_MAX;

    float derivative = (error - prev_error) / dt;
    prev_error = error;
    float torqueCmd = Kp_POS * error + Ki_POS * integral_error + Kd_POS * derivative;

    torqueCmd = constrain(torqueCmd, -TORQUE_MAX, TORQUE_MAX);

    //FILL IN IF THE sendCommand() FUNCTION HAS DIFFERENT ARGUMENTS
    motor1.sendCommand(0.0, 0.0, 0.0, 0.6, torqueCmd);

    const float POSITION_TOL = 0.03f;
    const unsigned long HOLD_TIME_MS = 1500;

    /*
    For your understanding, from the line "float pos = getMotor1PositionRad()" to "const unsigned long HOLD_TIME_MS = 1500;" does the following:
    1. Reads current motor position
    2. Gets current setpoint
    3. Integral term (with limit)
    4. Derivative term
    5. PID output
    6. Limit torque
    8. Sends command to motor
    9. Handles Setpoint switching
    */

    if (fabs(error) < POSITION_TOL) {
      if (now - lastSetpointReachedTime > HOLD_TIME_MS) {
        currentSetpointIndex++;
        if (currentSetpointIndex >= NUM_SETPOINTS) currentSetpointIndex = 0;
        integral_error = 0.0f;
        prev_error = 0.0f;
        lastSetpointReachedTime = now;
        Serial.print("Switched to setpoint index: ");
        Serial.println(currentSetpointIndex);
      }
    } else {
      lastSetpointReachedTime = now;
    }

    //Debug print
    Serial.print("SP: ");
    Serial.print(sp, 3);
    Serial.print(" | Pos: ");
    Serial.print(pos, 3);
    Serial.print(" | Error: ");
    Serial.print(error, 3);
    Serial.print(" | Torque: ");
    Serial.println(torqueCmd, 3);
  }
}
