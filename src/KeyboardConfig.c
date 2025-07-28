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
#ifndef DEBUG
const uint8_t NUM_KEYS_PER_ROW[NUM_ROWS] = {
    13, 15, 15, 14, 14, 10  // 75%, 81 keys
};
#else
const uint8_t NUM_KEYS_PER_ROW[NUM_ROWS] = {
    4, 4, 3, 3, 3, 3  // 75%, 81 keys
};
#endif

/****************************************************
 * KEYMAP: Map which MUX channel to which keycode?  *
 ****************************************************
 * Notes: Make sure the number of keycodes in this  *
 *        array matches NUM_KEYS!                   *
 *        The first entry maps to C0, second C1,    *
 *        and so on.                                *
 ****************************************************/
#ifndef DEBUG
const uint8_t KEYMAP_MATRIX[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW] = {
    {KC_ESCAPE, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12},
    {KC_GRAVE, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINUS, KC_EQUAL, KC_BACKSPACE, KC_DELETE},
    {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRACKET, KC_RBRACKET, KC_BSLASH, KC_END},
    {KC_CAPSLOCK, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCOLON, KC_QUOTE, KC_ENTER, KC_PGUP},
    {KC_LSHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLASH, KC_RSHIFT, KC_UP, KC_PGDOWN},
    {KC_LCTRL, KC_LGUI, KC_LALT, KC_SPACE, KC_RALT, KC_UNDEFINED, KC_RCTRL, KC_LEFT, KC_DOWN, KC_RIGHT}
};
#else
const uint8_t KEYMAP_MATRIX[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW] = {
    {KC_Q, KC_W, KC_E, KC_LSHIFT},
    {KC_R, KC_T, KC_Y, KC_SPACE},
    {KC_A, KC_S, KC_D},
    {KC_F, KC_G, KC_H},
    {KC_Z, KC_X, KC_C},
    {KC_V, KC_B, KC_N}
};
#endif

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