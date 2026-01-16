#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include <ACAN_ESP32.h>

/**
 * CANHandler — lightweight wrapper around ACAN_ESP32
 * --------------------------------------------------
 * ‣ Call setupCAN(txPin, rxPin) from your sketch’s setup().
 *   If no pins are passed, TX defaults to GPIO 22 and RX to GPIO 21.
 * ‣ DESIRED_BIT_RATE is set to 1 Mbit s⁻¹; adjust if needed.
 * ‣ latestFrame[0‑3] holds the most recent frame for motor IDs 1‑4.
 */

class CANHandler {
public:
    CANHandler() = default;

    /**
     * @brief Initializes the CAN bus interface with specified GPIO pins.
     * 
     * Configures the ACAN_ESP32 controller with the given transmit and receive pins,
     * sets the bit rate to DESIRED_BIT_RATE, and enables Normal mode operation.
     * Outputs success or error status to the serial port.
     * 
     * @param txPin The GPIO pin number to use for CAN transmission (typically GPIO 22)
     * @param rxPin The GPIO pin number to use for CAN reception (typically GPIO 21)
     * 
     * @return void
     */
    void setupCAN(uint8_t txPin = 22, uint8_t rxPin = 21);

    /**
     * @brief Transmits a CAN message onto the bus.
     * 
     * Attempts to send the provided CAN message using the ACAN_ESP32 controller's
     * non-blocking tryToSend() method. If the transmit buffer is full or the
     * transmission fails for any reason, an error message is logged to the serial
     * port for debugging purposes.
     * 
     * @param message The CANMessage to transmit, containing the message ID, data length,
     *                and payload bytes. Passed by const reference for efficiency.
     * 
     * @return void
     */
    void sendCANMessage(const CANMessage& message);

    /**
     * @brief Polls the CAN controller and caches the most recent frames.
     * 
     * Retrieves up to 4 incoming CAN messages from the ACAN_ESP32 receive buffer
     * and stores them in the latestFrame array. Updates the lastRecievedTime
     * timestamp for each received message to track device online status. This
     * method should be called regularly in the main loop() to ensure timely
     * processing of incoming CAN traffic.
     * 
     * @return void
     * 
     */
    void update();

    // Convenience helpers
    /**
     * @brief Checks if a device with the specified ID is currently online.
     * 
     * Determines online status by comparing the time elapsed since the last
     * received message from the device against the recieveTimeout threshold
     * (default 600ms). This is useful for detecting communication failures or
     * disconnected devices on the CAN bus.
     * 
     * @param id The device ID to check (1-4, corresponding to motor IDs).
     *           Internally mapped to array indices 0-3.
     * 
     * @return true if a message from this device was received within the timeout
     *         period; false if the device is offline or has not been heard from
     *         recently.
     */
    bool       getIsOnline(uint8_t id);
    
    /**
     * @brief Retrieves the most recently received CAN frame for a specified device ID.
     * 
     * Returns the last cached CAN message received from the device with the given ID.
     * The frame is stored during the update() method when messages are received from
     * the CAN bus. This provides quick access to the most recent data without needing
     * to poll the hardware directly.
     * 
     * @param id The device ID to retrieve the frame for (1-4, corresponding to motor IDs).
     *           Internally mapped to array indices 0-3.
     * 
     * @return CANMessage The most recent CAN frame received from the specified device.
     *         Returns the cached message from latestFrame[id-1].
     */
    CANMessage getLatestFrame(uint8_t id);
    
    /**
     * @brief Searches for and retrieves a CAN message by its message ID.
     * 
     * Searches through the latestFrame array to find a message with a matching
     * CAN message ID (not device index). This is useful when you need to retrieve
     * a specific message type based on the actual CAN ID field rather than the
     * device index position.
     * 
     * @param targetId The CAN message ID to search for (the id field of CANMessage,
     *                 not the device index). This is the actual identifier transmitted
     *                 on the CAN bus.
     * 
     * @return CANMessage The matching CAN message if found, or an empty/zeroed
     *         CANMessage struct if no message with the target ID exists in the cache.
     */
    CANMessage getLatestMessage(uint32_t targetId);
    
    /**
     * @brief Checks if a message with the specified CAN ID is online within a custom timeout.
     * 
     * Searches through all cached frames to find a message with the matching CAN message ID
     * and verifies that it was received within the specified timeout period. This provides
     * more flexible online status checking compared to getIsOnline(), allowing per-message
     * timeout values and searching by CAN message ID rather than device index.
     * 
     * @param targetId The CAN message ID to search for (the id field of CANMessage).
     * @param timeout  The maximum time in milliseconds since the message was received
     *                 to still consider it online.
     * 
     * @return true if a message with the target ID was found and received within the
     *         timeout period; false if no matching message exists or it has timed out.
     * 
     */
    bool       isMessageOnline(uint32_t targetId, uint32_t timeout) const;

    /**
     * @brief Array of the most recently received CAN frames.
     * 
     * Stores the 4 most recent CAN messages indexed 0-3, updated by the update()
     * method. Provides direct access to cached message data for all connected devices.
     */
    CANMessage latestFrame[4];

private:
    /// @brief CAN bus bit rate in bits per second (1 Mbps).
    static const uint32_t DESIRED_BIT_RATE = 1'000'000UL;

    /// @brief Timestamp (milliseconds) when each frame in latestFrame[0-3] was last received.
    uint32_t lastRecievedTime[4] = {0, 0, 0, 0};

    /// @brief Timeout in milliseconds; devices offline if no message received within this period.
    int recieveTimeout = 600;
};

#endif  // CAN_HANDLER_H
