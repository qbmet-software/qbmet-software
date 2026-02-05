#ifndef REMOTE_DEBUG_H
#define REMOTE_DEBUG_H

#include <Arduino.h>
#include <stdarg.h>

/**
 * @class RemoteDebug
 * @brief A minimal remote debugging interface for ESP32 systems.
 * 
 * This class provides a simple debugging utility that outputs formatted messages
 * over the serial interface. It is designed for remote monitoring and troubleshooting
 * of ESP32-based applications without requiring a full remote debugging framework.
 * 
 * The class supports:
 * - Initialization with a hostname identifier
 * - Line-based printing with automatic newline termination
 * - Printf-style formatted output for flexible message construction
 * 
 * Messages are routed to the Arduino Serial interface for transmission.
 */
class RemoteDebug {
public:

  /**
   * @brief Constructs a RemoteDebug instance.
   * 
   * Initializes the RemoteDebug object. The actual serial communication setup
   * should be performed in the begin() method.
   */
  RemoteDebug();

  /**
   * @brief Initializes the remote debug interface.
   * 
   * Sets up the remote debug system with an optional hostname identifier.
   * Prints a startup message to the serial interface.
   * 
   * @param hostname A string identifier for the device (e.g., "ESP32-Suit-Left-Leg")
   */
  void begin(const char* hostname);

  /**
   * @brief Prints a debug message with automatic newline termination.
   * 
   * Outputs the provided message to the serial interface followed by a newline.
   * @note This method is currently disabled in the implementation.
   * 
   * @param msg The message string to print
   */
  void println(const char* msg);

  /**
   * @brief Prints formatted debug output using printf-style formatting.
   * 
   * Provides C-style printf functionality for constructing complex debug messages
   * with variable substitution. Output is automatically terminated with a newline
   * and sent to the serial interface.
   * 
   * @param fmt Format string (follows standard printf conventions)
   * @param ... Variable arguments corresponding to format specifiers in fmt
   */
  void printf(const char* fmt, ...);
};

/**
 * @var Debug
 * @brief Global instance of the RemoteDebug class.
 * 
 * This global object is used throughout the application to output debug messages.
 * It can be accessed directly or through the debugI() and debugW() macros.
 */
extern RemoteDebug Debug;

/**
 * @def debugI(fmt, ...)
 * @brief Macro for informational debug messages.
 * 
 * Outputs informational-level messages using printf-style formatting.
 * Expands to a call to Debug.printf() with the provided format string and arguments.
 * 
 * @param fmt Format string (follows standard printf conventions)
 * @param ... Variable arguments corresponding to format specifiers
 */
#define debugI(fmt, ...) Debug.printf((fmt), ##__VA_ARGS__)

/**
 * @def debugW(fmt, ...)
 * @brief Macro for warning debug messages.
 * 
 * Outputs warning-level messages using printf-style formatting.
 * Expands to a call to Debug.printf() with the provided format string and arguments.
 * 
 * @param fmt Format string (follows standard printf conventions)
 * @param ... Variable arguments corresponding to format specifiers
 */
#define debugW(fmt, ...) Debug.printf((fmt), ##__VA_ARGS__)

#endif // REMOTE_DEBUG_H
