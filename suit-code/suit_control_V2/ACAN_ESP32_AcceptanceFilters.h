/**
 * @file ACAN_ESP32_AcceptanceFilters.h
 * @brief CAN message acceptance filter configuration for ESP32.
 * 
 * This header file defines the ACAN_ESP32_Filter class, which encapsulates CAN bus
 * message filtering functionality for the ACAN_ESP32 controller. It provides factory
 * methods for creating various filter configurations including single and dual filters
 * for both standard and extended CAN identifiers. The class manages acceptance code
 * and mask registers (ACR/AMR) that control which messages are accepted by the CAN
 * controller, enabling precise message filtering at the hardware level.
 * 
 * Based on the SJA1000 CAN controller datasheet specifications.
 * 
 */

#pragma once

//----------------------------------------------------------------------------------------

#include <Arduino.h>

//----------------------------------------------------------------------------------------

/**
 * @class ACAN_ESP32_Filter
 * @brief Configures CAN message acceptance filters for the ESP32 TWAI controller.
 * 
 * This class provides a mechanism to set up hardware-level message filtering for the
 * ACAN_ESP32 controller. It manages the Acceptance Code (ACR) and Acceptance Mask (AMR)
 * registers that determine which CAN messages are accepted based on their identifiers
 * and types. Filters can be configured for standard (11-bit) or extended (29-bit)
 * identifiers, and can differentiate between data and remote frames.
 * 
 * The filter configuration is based on the SJA1000 CAN controller architecture and
 * supports both single and dual filter modes.
 */
class ACAN_ESP32_Filter {

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @enum Format
   * @brief The CAN frame format mode for filtering.
   */
  public: typedef enum : uint8_t {
    standard,              ///< Accept only standard (11-bit) CAN identifiers
    extended,              ///< Accept only extended (29-bit) CAN identifiers
    standardAndExtended    ///< Accept both standard and extended CAN identifiers
  } Format ;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @enum Type
   * @brief The CAN frame type mode for filtering.
   */
  public: typedef enum : uint8_t {
    data,           ///< Accept only data frames
    remote,         ///< Accept only remote frames
    dataAndRemote   ///< Accept both data and remote frames
  } Type ;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // public properties
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  public: uint8_t mACR0 ;    ///< Acceptance Code Register 0
  public: uint8_t mACR1 ;    ///< Acceptance Code Register 1
  public: uint8_t mACR2 ;    ///< Acceptance Code Register 2
  public: uint8_t mACR3 ;    ///< Acceptance Code Register 3
  public: uint8_t mAMR0 ;    ///< Acceptance Mask Register 0
  public: uint8_t mAMR1 ;    ///< Acceptance Mask Register 1
  public: uint8_t mAMR2 ;    ///< Acceptance Mask Register 2
  public: uint8_t mAMR3 ;    ///< Acceptance Mask Register 3
  public: bool mAMFSingle ;  ///< True for single filter mode, false for dual filter mode
  public: Format mFormat ;   ///< The frame format (standard, extended, or both)

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Default private constructor
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @brief Default private constructor initializing filter registers.
   * 
   * Initializes all acceptance code registers to 0 and all mask registers to 0xFF
   * (no masking), effectively accepting all frames. This constructor is private;
   * use the static factory methods to create configured filters.
   */
  private: ACAN_ESP32_Filter (void) :
  mACR0 (0),
  mACR1 (0),
  mACR2 (0),
  mACR3 (0),
  mAMR0 (0xFF),
  mAMR1 (0xFF),
  mAMR2 (0xFF),
  mAMR3 (0xFF),
  mAMFSingle (false),
  mFormat (standardAndExtended) {
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Accept all filter
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @brief Creates a filter that accepts all CAN messages.
   * 
   * Configures the filter to accept all frame types and identifiers (both standard
   * and extended). This is useful for capturing all CAN traffic for debugging or
   * monitoring purposes.
   * 
   * @return ACAN_ESP32_Filter A filter configured to accept all messages.
   */
  public: static inline ACAN_ESP32_Filter acceptAll (void) {
    return ACAN_ESP32_Filter () ;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Accept only standard frames
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @brief Creates a filter that accepts only standard (11-bit) CAN frames.
   * 
   * Configures the filter to reject extended (29-bit) identifier frames while
   * accepting all standard identifier frames regardless of their content.
   * 
   * @return ACAN_ESP32_Filter A filter configured for standard frames only.
   */
  public: static inline ACAN_ESP32_Filter acceptStandardFrames (void) {
    ACAN_ESP32_Filter result ;
    result.mFormat = standard ;
    return result ;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Accept only extended frames
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @brief Creates a filter that accepts only extended (29-bit) CAN frames.
   * 
   * Configures the filter to reject standard (11-bit) identifier frames while
   * accepting all extended identifier frames regardless of their content.
   * 
   * @return ACAN_ESP32_Filter A filter configured for extended frames only.
   */
  public: static inline ACAN_ESP32_Filter acceptExtendedFrames (void) {
    ACAN_ESP32_Filter result ;
    result.mFormat = extended ;
    return result ;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // singleStandardFilter: see SJA100 datasheet, figure 9 page 45 (and figure 10 page 46)
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @brief Creates a single-mode filter for specific standard (11-bit) CAN identifiers.
   * 
   * Configures a single acceptance filter for standard identifiers. The filter will
   * accept only frames whose identifier matches the specified identifier within the
   * scope defined by the don't-care mask. See SJA1000 datasheet for detailed filtering
   * semantics.
   * 
   * @param inType The frame type to filter (data, remote, or both).
   * @param inIdentifier The CAN identifier to match (11-bit standard identifier).
   * @param inDontCareMask A mask indicating which bits are ignored during matching
   *                       (1 = ignore bit, 0 = match exactly).
   * 
   * @return ACAN_ESP32_Filter A single-mode filter for standard identifiers.
   */
  public: static inline ACAN_ESP32_Filter singleStandardFilter (const ACAN_ESP32_Filter::Type inType,
                                                                const uint16_t inIdentifier,
                                                                const uint16_t inDontCareMask) {
    ACAN_ESP32_Filter result ;
    result.mAMFSingle = true ; // Single Filter
    result.mFormat = standard ;
    result.mACR0 = uint8_t (inIdentifier >> 3) ;
    result.mACR1 = uint8_t (inIdentifier << 5) ;
    result.mAMR0 = uint8_t (inDontCareMask >> 3) ;
    result.mAMR1 = uint8_t (inDontCareMask << 5) | 0x0F ;
    switch (inType) {
    case data :
      break ;
    case remote :
      result.mACR1 |= 0x10 ;
      break ;
    case dataAndRemote :
      result.mAMR1 |= 0x10 ;
      break ;
    }
    return result ;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // singleExtendedFilter: see SJA100 datasheet, figure 10 page 46 (and figure 9 page 45)
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @brief Creates a single-mode filter for specific extended (29-bit) CAN identifiers.
   * 
   * Configures a single acceptance filter for extended identifiers. The filter will
   * accept only frames whose identifier matches the specified identifier within the
   * scope defined by the don't-care mask. See SJA1000 datasheet for detailed filtering
   * semantics.
   * 
   * @param inType The frame type to filter (data, remote, or both).
   * @param inIdentifier The CAN identifier to match (29-bit extended identifier).
   * @param inDontCareMask A mask indicating which bits are ignored during matching
   *                       (1 = ignore bit, 0 = match exactly).
   * 
   * @return ACAN_ESP32_Filter A single-mode filter for extended identifiers.
   */
  public: static inline ACAN_ESP32_Filter singleExtendedFilter (const ACAN_ESP32_Filter::Type inType,
                                                                const uint32_t inIdentifier,
                                                                const uint32_t inDontCareMask) {
    ACAN_ESP32_Filter result ;
    result.mAMFSingle = true ; // Single Filter
    result.mFormat = extended ;
    result.mACR0 = uint8_t (inIdentifier >> 21) ;
    result.mACR1 = uint8_t (inIdentifier >> 13) ;
    result.mACR2 = uint8_t (inIdentifier >> 5) ;
    result.mACR3 = uint8_t (inIdentifier << 3) ;

    result.mAMR0 = uint8_t (inDontCareMask >> 21) ;
    result.mAMR1 = uint8_t (inDontCareMask >> 13) ;
    result.mAMR2 = uint8_t (inDontCareMask >> 5) ;
    result.mAMR3 = uint8_t (inDontCareMask << 3) ;

    switch (inType) {
    case data :
      break ;
    case remote :
      result.mACR3 |= 0x04 ;
      break ;
    case dataAndRemote :
      result.mAMR3 |= 0x04 ;
      break ;
    }
    return result ;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // dualStandardFilter: see SJA100 datasheet, figure 11 page 47 (and figure 12 page 48)
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @brief Creates a dual-mode filter for two sets of standard (11-bit) CAN identifiers.
   * 
   * Configures two concurrent acceptance filters for standard identifiers in dual mode.
   * Frames matching either filter set will be accepted. This allows filtering for two
   * different identifier ranges simultaneously. See SJA1000 datasheet for detailed
   * filtering semantics.
   * 
   * @param inType0 The frame type to filter for the first filter set (data, remote, or both).
   * @param inIdentifier0 The CAN identifier to match for the first filter.
   * @param inDontCareMask0 The don't-care mask for the first filter.
   * @param inType1 The frame type to filter for the second filter set (data, remote, or both).
   * @param inIdentifier1 The CAN identifier to match for the second filter.
   * @param inDontCareMask1 The don't-care mask for the second filter.
   * 
   * @return ACAN_ESP32_Filter A dual-mode filter for standard identifiers.
   */
  public: static inline ACAN_ESP32_Filter dualStandardFilter (const ACAN_ESP32_Filter::Type inType0,
                                                              const uint16_t inIdentifier0,
                                                              const uint16_t inDontCareMask0,
                                                              const ACAN_ESP32_Filter::Type inType1,
                                                              const uint16_t inIdentifier1,
                                                              const uint16_t inDontCareMask1) {
    ACAN_ESP32_Filter result ;
    result.mAMFSingle = false ; // Dual Filter
    result.mFormat = standard ;

    result.mACR0 = uint8_t (inIdentifier0 >> 3) ;
    result.mACR1 = uint8_t (inIdentifier0 << 5) ;
    result.mAMR0 = uint8_t (inDontCareMask0 >> 3) ;
    result.mAMR1 = uint8_t (inDontCareMask0 << 5) | 0x0F ;
    switch (inType0) {
    case data :
      break ;
    case remote :
      result.mACR1 |= 0x10 ;
      break ;
    case dataAndRemote :
      result.mAMR1 |= 0x10 ;
      break ;
    }

    result.mACR2 = uint8_t (inIdentifier1 >> 3) ;
    result.mACR3 = uint8_t (inIdentifier1 << 5) ;
    result.mAMR2 = uint8_t (inDontCareMask1 >> 3) ;
    result.mAMR3 = uint8_t (inDontCareMask1 << 5) | 0x0F ;
    switch (inType1) {
    case data :
      break ;
    case remote :
      result.mACR3 |= 0x10 ;
      break ;
    case dataAndRemote :
      result.mAMR3 |= 0x10 ;
      break ;
    }
    return result ;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // dualExtendedFilter: see SJA100 datasheet, figure 12 page 48 (and figure 11 page 47)
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * @brief Creates a dual-mode filter for two sets of extended (29-bit) CAN identifiers.
   * 
   * Configures two concurrent acceptance filters for extended identifiers in dual mode.
   * Frames matching either filter set will be accepted. This allows filtering for two
   * different identifier ranges simultaneously. See SJA1000 datasheet for detailed
   * filtering semantics.
   * 
   * @param inIdentifier0 The CAN identifier to match for the first filter.
   * @param inDontCareMask0 The don't-care mask for the first filter.
   * @param inIdentifier1 The CAN identifier to match for the second filter.
   * @param inDontCareMask1 The don't-care mask for the second filter.
   * 
   * @return ACAN_ESP32_Filter A dual-mode filter for extended identifiers.
   */
  public: static inline ACAN_ESP32_Filter dualExtendedFilter (const uint32_t inIdentifier0,
                                                              const uint32_t inDontCareMask0,
                                                              const uint32_t inIdentifier1,
                                                              const uint32_t inDontCareMask1) {
    ACAN_ESP32_Filter result ;
    result.mAMFSingle = false ; // Dual Filter
    result.mFormat = extended ;

    result.mACR0 = uint8_t (inIdentifier0 >> 21) ;
    result.mACR1 = uint8_t (inIdentifier0 >> 13) ;
    result.mAMR0 = uint8_t (inDontCareMask0 >> 21) ;
    result.mAMR1 = uint8_t (inDontCareMask0 >> 13) ;

    result.mACR2 = uint8_t (inIdentifier1 >> 21) ;
    result.mACR3 = uint8_t (inIdentifier1 << 13) ;
    result.mAMR2 = uint8_t (inDontCareMask1 >> 21) ;
    result.mAMR3 = uint8_t (inDontCareMask1 << 13) ;

    return result ;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} ;

//----------------------------------------------------------------------------------------
