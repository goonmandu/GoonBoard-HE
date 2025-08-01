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
#ifndef PROTO_BOARD
const uint8_t NUM_KEYS_PER_ROW[NUM_ROWS] = {
    13, 15, 15, 14, 14, 10  // 75%, 81 keys
};
#else
const uint8_t NUM_KEYS_PER_ROW[NUM_ROWS] = {
    4, 4, 3, 3, 3, 3  // Development Only
};
#endif

// Allocate and zero-initialize the keymap and actuation matrices
volatile uint8_t KEYMAP_MATRIX[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW] = { { 0 } };
volatile uint8_t ACTUATIONS_MATRIX[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW] = { { 0 } };

// Allocate the rapid-trigger thresholds
volatile uint8_t RAPID_TRIGGER_THRESHOLD = 0;
volatile uint8_t RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD = 0;

// Define the rapid-trigger config consts
const uint8_t RAPID_TRIGGER_IDLE_HYSTERESIS = MS(35);
const uint8_t RAPID_TRIGGER_DIRECTION_HYSTERESIS = MS(3);

// Allocate the snap tap config bytes
volatile uint8_t SNAPTAP_A_STATUS = SNAPTAP_DISABLED;
volatile uint8_t SNAPTAP_A_KEY1_COORDS = SNAPTAP_KEYCOORDS(0, 0);
volatile uint8_t SNAPTAP_A_KEY2_COORDS = SNAPTAP_KEYCOORDS(0, 0);
volatile uint8_t SNAPTAP_B_STATUS = SNAPTAP_DISABLED;
volatile uint8_t SNAPTAP_B_KEY1_COORDS = SNAPTAP_KEYCOORDS(0, 0);
volatile uint8_t SNAPTAP_B_KEY2_COORDS = SNAPTAP_KEYCOORDS(0, 0);
