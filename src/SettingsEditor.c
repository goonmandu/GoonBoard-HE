#include <avr/eeprom.h>
#include "SettingsEditor.h"
#include "DefaultSettings.h"

void load_eeprom_to_sram(void) {
    // Load EEPROM keymap to SRAM
    eeprom_read_block((void*)KEYMAP_MATRIX, (const void*)&default_settings.keymap, MAX_KEYS_SUPPORTED);

    // Load EEPROM actuations to SRAM
    eeprom_read_block((void*)ACTUATIONS_MATRIX, (const void*)&default_settings.actuations, MAX_KEYS_SUPPORTED);

    // Load EEPROM RT threshold to SRAM
    RAPID_TRIGGER_THRESHOLD = eeprom_read_byte((const uint8_t*)&default_settings.rt_threshold);

    // Load EEPROM RT SC threshold to SRAM
    RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD = eeprom_read_byte((const uint8_t*)&default_settings.rt_sc_threshold);
}


void save_keymap_to_eeprom(uint8_t row, uint8_t key, uint8_t keycode) {
    eeprom_update_byte(
        (uint8_t*)(&(default_settings.keymap[row][key])),
        keycode
    );
}

void save_actuation_to_eeprom(uint8_t row, uint8_t key, uint8_t actuation) {
    eeprom_update_byte(
        (uint8_t*)(&(default_settings.actuations[row][key])),
        actuation
    );
}

void save_rt_threshold_to_eeprom(uint8_t rt_threshold) {
    eeprom_update_byte(
        (uint8_t*)(&default_settings.rt_threshold),
        rt_threshold
    );
}

void save_rt_sc_threshold_to_eeprom(uint8_t rt_sc_threshold) {
    eeprom_update_byte(
        (uint8_t*)(&default_settings.rt_sc_threshold),
        rt_sc_threshold
    );
}
