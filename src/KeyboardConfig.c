#include <stdint.h>
#include "KeyboardConfig.h"
#include "Keycodes.h"

/***************************************************
 *      ┌──────────────────────────────────┐       *
 *      | KEYBOARD CONFIGURATION CONSTANTS |       *
 *      └──────────────────────────────────┘       *
 *     Please read KeyboardConfig.h before any     *
 *   customization as macros are very important!   *
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
/*
const uint8_t NUM_KEYS_PER_ROW[NUM_ROWS] = {
    13, 15, 15, 14, 14, 10  // 75%, 81 keys
};
 */
const uint8_t NUM_KEYS_PER_ROW[NUM_ROWS] = {
    4, 4, 3, 3, 3, 3  // 75%, 81 keys
};

/****************************************************
 * KEYMAP: Map which MUX channel to which keycode?  *
 ****************************************************
 * Notes: Make sure the number of keycodes in this  *
 *        array matches NUM_KEYS!                   *
 *        The first entry maps to C0, second C1,    *
 *        and so on.                                *
 ****************************************************/
const uint8_t KEYMAP_MATRIX[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW] = {
    {KC_Q, KC_W, KC_E, KC_LSHIFT},
    {KC_R, KC_T, KC_Y, KC_SPACE},
    {KC_A, KC_S, KC_D},
    {KC_F, KC_G, KC_H},
    {KC_Z, KC_X, KC_C},
    {KC_V, KC_B, KC_N}
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
const uint8_t COMMON_ACTUATION = MM(1.5);

// To allow per-key actuation, go to `include/KeyboardConfig.h`
// and comment out `#define USE_COMMON_ACTUATION`.
#ifndef USE_COMMON_ACTUATION
const uint8_t ACTUATIONS_MATRIX[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW] = {
    {MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(6.9), MM(6.9), MM(6.9), MM(6.9),
     MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9)},
     
    {MM(1.5), MM(1.5), MM(1.5), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9),
     MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9)},
     
    {MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(6.9), MM(6.9), MM(6.9), MM(6.9),
     MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9)},
     
    {MM(1.5), MM(1.5), MM(1.5), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9),
     MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9)},
     
    {MM(1.5), MM(1.5), MM(1.5), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9),
     MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9)},
     
    {MM(1.5), MM(1.5), MM(1.5), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9),
     MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9), MM(6.9)}
};
#endif /* USE_COMMON_ACTUATION */

// Rapid Trigger
const uint8_t RAPID_TRIGGER_THRESHOLD =                 MM(0.3);
const uint8_t RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD =   MM(0.3);
const uint8_t RAPID_TRIGGER_IDLE_HYSTERESIS =           MS(35);
const uint8_t RAPID_TRIGGER_DIRECTION_HYSTERESIS =      MS(3);