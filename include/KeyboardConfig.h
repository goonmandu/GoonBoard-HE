#ifndef KEYBOARDCONFIG_H
#define KEYBOARDCONFIG_H

#include "Keyboard.h"


/***************************************************
 *      ┌──────────────────────────────────┐       *
 *      |       USE_COMMON_ACTUATION       |       *
 *      └──────────────────────────────────┘       *
 *       Uncomment the line below to enable        *
 *           per-key actuation points!             *
 ***************************************************/
#define USE_COMMON_ACTUATION

/////////////////////////////////////////////////////////
//           ┌───────────────────┐
//           |  SYSTEM CRITICAL  |
//           └───────────────────┘
// THIS FIRMWARE, AT 16 MHz, CANNOT SUPPORT 96 KEYS
// DOING RAPID TRIGGER AT 1000 Hz POLLING RATE!
//
// TESTED WORKING MAXIMUM KEY COUNT IS 81 KEYS!
// DO NOT CHANGE THESE TWO NUMBERS!
//
// ARBITRARILY INCREASING THESE NUMBERS MAY
// BREAK RAPID TRIGGER AND/OR USB COMPLIANCE!
//
/* DO NOT CHANGE */ #define NUM_ROWS 6
/* DO NOT CHANGE */ #define MAX_KEYS_SUPPORTED_PER_ROW 16
/* DO NOT CHANGE */ #define MAX_KEYS_SUPPORTED (NUM_ROWS * MAX_KEYS_SUPPORTED_PER_ROW)
//////////////////////////////////////////////////////////


// This is defined as the required keypress depth.
// For example, MM(0.5) means you have to press the key 0.5 mm for it to register as a press.
// Valid range: 0.1 - 3.4 (for Gateron "Magnetic Jade Shaft" KS-20T switches)
// ADC baselines when switch makes physical contact with sensor: 
//     104 (0x68) when switch is fully released.
//      68 (0x22) when switch is fully pressed.
// Conversion factor: 1 ADC bit ≈ 0.1 mm travel for KS-20T.
//
// Notes:
//   - Total travel: 3.5 mm
//     (104 - 68) / 35 = ~1.03 bits per 0.1 mm
//     Rounded to 1 bit per 0.1 mm for simplicity
#define MM(MILLIMETERS) ((uint8_t)(MILLIMETERS*10))

// Each USB frame happens every 1 ms.
#define MS(MILLISECONDS) (MILLISECONDS)

extern const uint8_t NUM_KEYS_PER_ROW[NUM_ROWS];
extern volatile uint8_t KEYMAP_MATRIX[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern volatile uint8_t ACTUATIONS_MATRIX[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];

extern volatile uint8_t RAPID_TRIGGER_THRESHOLD;
extern volatile uint8_t RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD;
extern const uint8_t RAPID_TRIGGER_IDLE_HYSTERESIS;
extern const uint8_t RAPID_TRIGGER_DIRECTION_HYSTERESIS;

#define SNAPTAP_DISABLED 0
#define SNAPTAP_ENABLED 1
#define SNAPTAP_KEYCOORDS(ROW, KEY) ((uint8_t)((ROW << 4) | KEY))
extern volatile uint8_t SNAPTAP_STATUS;         // Either 0 or 1
extern volatile uint8_t SNAPTAP_KEY1_COORDS;    // Packed nibbles ROW#:KEY# (MSb - LSb)
extern volatile uint8_t SNAPTAP_KEY2_COORDS;    // Packed nibbles ROW#:KEY# (MSb - LSb)

#endif /* KEYBOARDCONFIG_H */
