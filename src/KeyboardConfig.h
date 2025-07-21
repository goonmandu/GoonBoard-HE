#ifndef KEYBOARDCONFIG_H
#define KEYBOARDCONFIG_H

#include "Keyboard.h"

// This is defined as the required keypress depth.
// For example, MM(0.5) means you have to press the key 0.5 mm for it to register as a press.
// Valid range: 0.1 - 3.4 (for Gateron "Magnetic Jade Shaft" KS-20T switches)
// ADC baseline: 100 (0x64) when switch is fully released.
// Conversion factor: 1 ADC bit ≈ 0.1 mm travel for KS-20T.
//
// Notes:
//   - Released ADC: 104
//   - Bottom-out ADC: 68
//   - Total travel: 3.5 mm
//
//     (104 - 68) / 35 = ~1.03 bits per mm
//     Rounded to 1 bit per 0.1 mm for simplicity
#define MM(MILLIMETERS) (100-(int)(MILLIMETERS*10))

// Refer to "lib/LUFA/Drivers/USB/Class/Common/HIDClassCommon.h" for the key names.
// The KEY(NAME) macro just appends "HID_KEYBOARD_SC_" to the NAME argument.
#define KEY(NAME) HID_KEYBOARD_SC_##NAME

/* Customize your keys here */

// How many keys? 1 - 16.
#define NUM_KEYS 4

// Use the KEY(NAME) macro.
// MUX Channels C0 - C15, in order
const uint8_t KEYMAP[NUM_KEYS] = {
	KEY(A),
    KEY(S),
    KEY(D),
    KEY(F),
};

// Use the MM() macro.
// Uncomment the following line to use one actuation point across all switches.
// #define USE_COMMON_ACTUATION
#define COMMON_ACTUATION MM(1.5)

// Use the MM() macro.
// MUX Channels C0 - C15, in order
#ifndef USE_COMMON_ACTUATION
const uint8_t ACTUATIONS[NUM_KEYS] = {
	MM(1.0),
	MM(1.5),
	MM(2.0),
	MM(2.5),
};
#endif /* USE_COMMON_ACTUATION */

#endif /* KEYBOARDCONFIG_H */