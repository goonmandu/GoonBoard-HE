/* This file is only relevant when flashing the MCU. */

#include "DefaultSettings.h"
#include "Keycodes.h"

keyboard_settings_t EEMEM default_settings = {
    #ifndef PROTO_BOARD
    .keymap = {
        {KC_ESCAPE, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12},
        {KC_GRAVE, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINUS, KC_EQUAL, KC_BACKSPACE, KC_DELETE},
        {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRACKET, KC_RBRACKET, KC_BSLASH, KC_END},
        {KC_CAPSLOCK, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCOLON, KC_QUOTE, KC_ENTER, KC_PGUP},
        {KC_LSHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLASH, KC_RSHIFT, KC_UP, KC_PGDOWN},
        {KC_LCTRL, KC_LGUI, KC_LALT, KC_SPACE, KC_RALT, KC_UNDEFINED, KC_RCTRL, KC_LEFT, KC_DOWN, KC_RIGHT}
    },
    #else
    .keymap = {
        {KC_Q, KC_W, KC_E, KC_BACKSPACE},
        {KC_R, KC_T, KC_Y, KC_SPACE},
        {KC_A, KC_S, KC_D},
        {KC_F, KC_G, KC_H},
        {KC_Z, KC_X, KC_C},
        {KC_V, KC_B, KC_N}
    },
    #endif

    .actuations = {
        {MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5),
         MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5)},
        
        {MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5),
         MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5)},
        
        {MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5),
         MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5)},
        
        {MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5),
         MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5)},
        
        {MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5),
         MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5)},
        
        {MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5),
         MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5), MM(1.5)}
    },

    .rotary_counterclockwise = KC_MEDIA_VOL_DOWN,
    .rotary_clockwise = KC_MEDIA_VOL_UP,
    .rotary_pushbutton = KC_MEDIA_PLAY,

    .rt_status = false,
    .rt_threshold = MM(0.5),
    .rt_sc_threshold = MM(0.5),

    .snaptap_a_status = SNAPTAP_ENABLED,
    .snaptap_a_key1 = SNAPTAP_KEYCOORDS(3, 1),
    .snaptap_a_key2 = SNAPTAP_KEYCOORDS(3, 3),

    .snaptap_b_status = SNAPTAP_ENABLED,
    .snaptap_b_key1 = SNAPTAP_KEYCOORDS(2, 2),
    .snaptap_b_key2 = SNAPTAP_KEYCOORDS(3, 2),
};