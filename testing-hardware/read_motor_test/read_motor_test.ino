#include <Arduino.h>
#include "CANHandler.h"
#include "Motor.h"
#include "RemoteDebug.h"

// --- Configuration ---
#define CAN_TX_PIN 22
#define CAN_RX_PIN 21
#define MOTOR_ID   4  // Set this to your motor's CAN ID

// --- Global Instances ---
CANHandler canHandler;
// RemoteDebug is already declared as an 'extern' in RemoteDebug.h 
// and instantiated in RemoteDebug.cpp

// Initialize motor with ID, the handler, and the debug instance
Motor myMotor(MOTOR_ID, canHandler, Debug);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for serial monitor

  Serial.println("--- Motor Position Reader ---");

  // 1. Initialize CAN hardware
  canHandler.setupCAN(CAN_TX_PIN, CAN_RX_PIN);

  // 2. Start the motor (Sends the 0xFC command to enter MIT mode)
  Serial.print("Starting motor ID: ");
  Serial.println(MOTOR_ID);
  myMotor.start(); 
}

void loop() {
  // 3. Poll the CAN bus for new messages
  canHandler.update();

  // 4. Process the latest messages for the motor (unpacking data)
  myMotor.update();

  // 5. Check if the motor is sending data and print position
  if (myMotor.isOnline()) {
    float position = myMotor.getPosition();
    float velocity = myMotor.getVelocity();
    
    Serial.print("Motor ID: ");
    Serial.print(MOTOR_ID);
    Serial.print(" | Position: ");
    Serial.print(position, 4); // Print with 4 decimal places
    Serial.println(" rad");
    Serial.print(" | Velocity: ");
    Serial.print(velocity, 4); // Print with 4 decimal places
    Serial.println(" rad/s");


    // Target velocity in radians/second (e.g., 0.5 rad/s is quite slow)
    float targetVelocity = 0.5; 
    // Damping gain (kd). 
    // Start low (e.g., 0.1 to 1.0) to avoid high-frequency oscillations.
    float kd = 1.0; 
    myMotor.setVelocity(targetVelocity, kd);
    
  } else {
    Serial.println("Motor Offline... check wiring and ID.");
  }

  // Add a small delay so the Serial monitor is readable
  delay(100);
}
