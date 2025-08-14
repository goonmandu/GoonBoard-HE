/* This file is only relevant when flashing the MCU. */

#ifndef DEFAULTSETTINGS_H
#define DEFAULTSETTINGS_H


#include "KeyboardConfig.h"

typedef struct {
    uint8_t keymap[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
    uint8_t actuations[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];

    uint8_t rotary_counterclockwise;
    uint8_t rotary_clockwise;
    uint8_t rotary_pushbutton;

    uint8_t rt_status;
    uint8_t rt_threshold;
    uint8_t rt_sc_threshold;

    uint8_t snaptap_a_status;
    uint8_t snaptap_a_key1;
    uint8_t snaptap_a_key2;

    uint8_t snaptap_b_status;
    uint8_t snaptap_b_key1;
    uint8_t snaptap_b_key2;
} keyboard_settings_t;

extern EEMEM keyboard_settings_t default_settings;


#endif