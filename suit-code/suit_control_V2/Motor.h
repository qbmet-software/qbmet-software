/**
 * @file Motor.h
 * @brief High-level interface for brushless motor control via CAN bus.
 * 
 * This header file defines the Motor class, which provides a convenient abstraction
 * for commanding and monitoring brushless motor controllers over the CAN interface.
 * The Motor class handles conversion between physical units and CAN message formats,
 * manages motor state tracking, and enforces constraint ranges on all control parameters.
 * 
 */

#ifndef MOTOR_H
#define MOTOR_H

//#include "ACAN_ESP32.h"
#include "CANHandler.h"
#include "RemoteDebug.h"

/**
 * @enum CAN_PACKET_ID
 * @brief Control mode identifiers for CAN communication with motor controllers.
 * 
 * Defines the different control modes available for commanding the motor through
 * the CAN interface. Each mode represents a different control strategy.
 */
typedef enum {
  CAN_PACKET_SET_DUTY = 0,           ///< Duty Cycle Mode - Direct PWM control
  CAN_PACKET_SET_CURRENT,            ///< Current Loop Mode - Current feedback control
  CAN_PACKET_SET_CURRENT_BRAKE,      ///< Current Brake Mode - Braking with current control
  CAN_PACKET_SET_RPM,                ///< Speed Mode - Velocity control
  CAN_PACKET_SET_POS,                ///< Position Mode - Direct position control
  CAN_PACKET_SET_ORIGIN_HERE,        ///< Set Origin Mode - Calibrate current position as zero
  CAN_PACKET_SET_POS_SPD             ///< Position-Speed Loop Mode - Combined position and velocity control
} CAN_PACKET_ID;

/**
 * @class Motor
 * @brief Interface for controlling a brushless motor via CAN bus communication.
 * 
 * This class provides a high-level abstraction for commanding and monitoring a
 * brushless motor controller connected to the CAN bus. It handles the conversion
 * between physical units (position, velocity, torque) and CAN message formats,
 * manages communication through a CANHandler, and tracks motor state including
 * position, velocity, torque, temperature, and error conditions.
 * 
 * The motor operates with constrained ranges for all control parameters. Commands
 * are sent at a configurable interval (default 1ms) and incoming status updates
 * are processed through the update() method.
 */
class Motor
{
public:
  /**
   * @brief Constructs a Motor instance and associates it with a CAN bus and debug interface.
   * 
   * @param ID The unique CAN identifier for this motor (typically 1-4).
   * @param canHandler Reference to the CANHandler managing CAN bus communication.
   * @param Debug Reference to the RemoteDebug instance for debug output.
   */
  Motor(uint16_t ID, CANHandler &canHandler, RemoteDebug &Debug);

  /**
   * @brief Starts the motor (sends initialization command).
   * 
   * Prepares the motor for operation by sending the appropriate startup command.
   * 
   * @return void
   */
  void start();

  /**
   * @brief Stops the motor (sends stop command).
   * 
   * Halts motor operation by sending a stop command via CAN.
   * 
   * @return void
   */
  void stop();

  /**
   * @brief Commands the motor to a target position with proportional and derivative gains.
   * 
   * Sets the desired position and feedback control parameters. The motor will use
   * proportional and derivative feedback to reach and maintain the target position.
   * 
   * @param pos The target position in physical units (range: P_MIN to P_MAX).
   * @param kp The proportional gain coefficient (range: Kp_MIN to Kp_MAX).
   * @param kd The derivative gain coefficient (range: Kd_MIN to Kd_MAX).
   * 
   * @return void
   */
  void setPosition(float pos, float kp, float kd);

  /**
   * @brief Commands the motor to a target velocity with derivative damping.
   * 
   * Sets the desired velocity with damping control. The motor will accelerate/
   * decelerate to reach the target velocity.
   * 
   * @param vel The target velocity in physical units (range: V_MIN to V_MAX).
   * @param kd The derivative damping coefficient (range: Kd_MIN to Kd_MAX).
   * 
   * @return void
   */
  void setVelocity(float vel, float kd);

  /**
   * @brief Commands the motor with a feedforward torque value.
   * 
   * Directly commands the motor with a torque value, useful for open-loop control
   * or as a feedforward term in a control system.
   * 
   * @param torque The target torque in physical units (range: T_MIN to T_MAX).
   * 
   * @return void
   */
  void setTorque(float torque);

  /**
   * @brief Re-zeros the motor's position encoder.
   * 
   * Resets the motor's position reference point to the current position,
   * effectively setting position to zero.
   * 
   * @return void
   */
  void reZero();

  /**
   * @brief Sends a comprehensive motor command with all control parameters.
   * 
   * Sends a command using the existing conversion functions and control mode.
   * This method allows simultaneous specification of position, velocity, and torque
   * setpoints along with proportional and derivative gains.
   * 
   * @param p_des Desired position (range: P_MIN to P_MAX).
   * @param v_des Desired velocity (range: V_MIN to V_MAX).
   * @param kp Proportional gain (range: Kp_MIN to Kp_MAX).
   * @param kd Derivative gain (range: Kd_MIN to Kd_MAX).
   * @param t_ff Feedforward torque (range: T_MIN to T_MAX).
   * 
   * @return void
   */
  void sendCommand(float p_des, float v_des, float kp, float kd, float t_ff);

  /**
   * @brief Commands a braking current to the motor.
   * 
   * Activates current brake mode with the specified braking current.
   * The value (in Amperes) is scaled internally: 1 A = 1000 counts.
   * 
   * @param current The braking current in Amperes.
   * 
   * @return void
   */
  void setCurrentBrake(float current);

  /**
   * @brief Retrieves the current motor position from the last received status frame.
   * 
   * Returns the most recent position value received from the motor controller.
   * Position is expressed in the motor's physical units (typically radians or rotations).
   * 
   * @return float The current position.
   */
  float getPosition() const;

  /**
   * @brief Retrieves the current motor velocity from the last received status frame.
   * 
   * Returns the most recent velocity value received from the motor controller.
   * Velocity is expressed in the motor's physical units per unit time.
   * 
   * @return float The current velocity.
   */
  float getVelocity() const;

  /**
   * @brief Retrieves the current motor torque from the last received status frame.
   * 
   * Returns the most recent torque/current value received from the motor controller.
   * 
   * @return float The current torque.
   */
  float getTorque() const;

  /**
   * @brief Retrieves the motor controller's current temperature.
   * 
   * Returns the last received temperature value from the motor controller.
   * Uses a signed integer type to ensure proper arithmetic operations.
   * 
   * @return int The motor temperature in degrees Celsius.
   */
  int getTemperature() const;

  /**
   * @brief Retrieves the motor controller's error code.
   * 
   * Returns the latest error status from the motor controller. A value of 0
   * typically indicates no error. Non-zero values indicate specific fault conditions.
   * 
   * @return uint8_t The error code.
   */
  uint8_t getErrorCode() const;

  /**
   * @brief Checks if the motor is currently responding to CAN messages.
   * 
   * Determines online status based on communication timeout. Returns true if a
   * status message has been received within the configured timeout period.
   * 
   * @return bool True if the motor is online and communicating; false otherwise.
   */
  bool isOnline() const;

  /**
   * @brief Updates motor state from the most recent CAN message.
   * 
   * Polls the CANHandler for new incoming messages for this motor and processes
   * them to update internal state (position, velocity, torque, temperature, error code).
   * Should be called regularly in the main control loop.
   * 
   * @return void
   */
  void update();

private:
  uint16_t canID;                           ///< CAN identifier for this motor.

  // Physical constraint ranges for motor control parameters
  float P_MIN = -40;                        ///< Minimum position limit.
  float P_MAX = 40;                         ///< Maximum position limit.
  float V_MIN = -50;                        ///< Minimum velocity limit.
  float V_MAX = 50;                         ///< Maximum velocity limit.
  float T_MIN = -25;                        ///< Minimum torque limit.
  float T_MAX = 25;                         ///< Maximum torque limit.
  float Kp_MIN = 0;                         ///< Minimum proportional gain.
  float Kp_MAX = 500;                       ///< Maximum proportional gain.
  float Kd_MIN = 0;                         ///< Minimum derivative gain.
  float Kd_MAX = 5;                         ///< Maximum derivative gain.

  // Motor state variables from last received status frame
  float pOut = 0;                           ///< Current position (from last status update).
  float vOut = 0;                           ///< Current velocity (from last status update).
  float iOut = 0;                           ///< Current torque/current (from last status update).
  int temperature = 0;                      ///< Motor temperature in degrees Celsius (signed for proper arithmetic).
  uint8_t errorCode = 0;                    ///< Error code from motor controller.
  bool isStopped;                           ///< Motor stopped state flag.

  /**
   * @brief Converts a floating-point value to an integer representation.
   * 
   * Maps a floating-point value within a specified range to an unsigned integer,
   * scaling by the specified number of bits. Used for CAN message encoding.
   * 
   * @param x The floating-point value to convert.
   * @param x_min The minimum value of the floating-point range.
   * @param x_max The maximum value of the floating-point range.
   * @param bits The number of bits to use for the integer representation.
   * 
   * @return int The encoded integer value.
   */
  int floatToUInt(float x, float x_min, float x_max, unsigned int bits);

  /**
   * @brief Converts an integer representation back to a floating-point value.
   * 
   * Inverse of floatToUInt(). Maps an unsigned integer back to a floating-point
   * value within the specified range. Used for CAN message decoding.
   * 
   * @param x_int The encoded integer value.
   * @param x_min The minimum value of the floating-point range.
   * @param x_max The maximum value of the floating-point range.
   * @param bits The number of bits used in the integer representation.
   * 
   * @return float The decoded floating-point value.
   */
  float uintToFloat(int x_int, float x_min, float x_max, int bits);

  /**
   * @brief Encodes control parameters into a CAN message.
   * 
   * Packs the provided position, velocity, and torque setpoints along with
   * control gains into the CAN message format expected by the motor controller.
   * 
   * @param msg Reference to the CANMessage to populate.
   * @param p_des Desired position.
   * @param v_des Desired velocity.
   * @param kp Proportional gain.
   * @param kd Derivative gain.
   * @param t_ff Feedforward torque.
   * 
   * @return void
   */
  void packCommand(CANMessage &msg, float p_des, float v_des, float kp, float kd, float t_ff);

  /**
   * @brief Decodes a CAN status message from the motor controller.
   * 
   * Unpacks the position, velocity, torque, temperature, and error information
   * from an incoming CAN message and updates the corresponding member variables.
   * 
   * @param msg The incoming CAN status message from the motor controller.
   * 
   * @return void
   */
  void unpackCommand(const CANMessage &msg);

  CANMessage latestFrame;                   ///< Most recently sent outgoing command frame.
  CANHandler &canHandler;                   ///< Reference to the CAN bus handler.
  uint32_t lastSendTime = 0;                ///< Timestamp of the last command sent (milliseconds).
  int sendInterval = 1;                     ///< Interval between command transmissions (milliseconds).
  RemoteDebug &Debug;                       ///< Reference to the debug logging interface.
};

#endif // MOTOR_H
