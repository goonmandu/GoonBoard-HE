#ifndef KEYBOARDCONFIG_H
#define KEYBOARDCONFIG_H

#include "Keyboard.h"

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

// Refer to "lib/LUFA/Drivers/USB/Class/Common/HIDClassCommon.h" for the key names.
// The KEY(NAME) macro just appends "HID_KEYBOARD_SC_" to the NAME argument.
#define KEY(NAME) HID_KEYBOARD_SC_##NAME



/***************************************************
 *      ┌──────────────────────────────────┐       *
 *      | KEYBOARD CONFIGURATION CONSTANTS |       *
 *      └──────────────────────────────────┘       *
 * Please read the stuff ^^up there^^ first before *
 *            doing any configuration!             *
 ***************************************************/


/***************************************************
 * NUM_KEYS: How many keys on your keyboard?       *
 * Minimum:  1                                     *
 * Maximim: 16                                     *
 ***************************************************
 * Notes: If you change this, make sure to also    *
 *        change the number of elements on the     *
 *        keymap and actuation arrays!             *
 ***************************************************/
#define NUM_KEYS 4


/****************************************************
 * KEYMAP: Map which MUX channel to which keycode?  *
 ****************************************************
 * Notes: Make sure the number of keycodes in this  *
 *        array matches NUM_KEYS!                   *
 *        The first entry maps to C0, second C1,    *
 *        and so on.                                *
 ****************************************************/
const uint8_t KEYMAP[NUM_KEYS] = {
    KEY(A),
    KEY(S),
    KEY(D),
    KEY(F),
};


/****************************************************
 * COMMON_ACTUATION: Actuation point for all keys?  *
 * Minimum: MM(0.1)                                 *
 * Maximim: MM(3.4)                                 *
 ****************************************************
 * Notes: This is specific to Gateron Jade stems,   *
 *        which have a 3.5mm travel distance.       *
 *        Uncomment USE_COMMON_ACTUATION to enable  *
 *        common actuation instead of per-key       *
 *        actuation.                                *
 ****************************************************/
#define USE_COMMON_ACTUATION
#define COMMON_ACTUATION \
        MM(1.5)


/****************************************************
 * ACTUATIONS: Actuation point for each key?        *
 ****************************************************
 * Notes: Make sure the number of actuation points  *
 *        in this array matches NUM_KEYS!           *
 *        The first entry maps to C0, second C1,    *
 *        and so on.                                *
 ****************************************************/
#ifndef USE_COMMON_ACTUATION
const uint8_t ACTUATIONS[NUM_KEYS] = {
    MM(1.0),
    MM(1.5),
    MM(2.0),
    MM(2.5),
};
#endif /* USE_COMMON_ACTUATION */


/*****************************************************
 * RAPID_TRIGGER Configurations                      *
 *****************************************************
 * Notes: TODO                                       *
 *****************************************************/
#define RAPID_TRIGGER_THRESHOLD \
        MM(0.3)

#define RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD \
        MM(0.5)

#define RAPID_TRIGGER_IDLE_HYSTERESIS \
        MS(35)

#define RAPID_TRIGGER_DIRECTION_HYSTERESIS \
        MS(2)


#endif /* KEYBOARDCONFIG_H */