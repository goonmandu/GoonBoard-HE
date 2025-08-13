#include <avr/eeprom.h>
#include "SettingsEditor.h"
#include "DefaultSettings.h"
#include "SnapTap.h"

void load_eeprom_to_sram(void) {
    // Load EEPROM keymap to SRAM
    eeprom_read_block((void*)KEYMAP_MATRIX, (const void*)&default_settings.keymap, MAX_KEYS_SUPPORTED);

    // Load EEPROM actuations to SRAM
    eeprom_read_block((void*)ACTUATIONS_MATRIX, (const void*)&default_settings.actuations, MAX_KEYS_SUPPORTED);

    ROTARY_COUNTERCLOCKWISE_KEYMAP = eeprom_read_byte((const uint8_t*)&default_settings.rotary_counterclockwise);
    ROTARY_CLOCKWISE_KEYMAP = eeprom_read_byte((const uint8_t*)&default_settings.rotary_clockwise);
    ROTARY_PUSHBUTTON_KEYMAP = eeprom_read_byte((const uint8_t*)&default_settings.rotary_pushbutton);

    // Load EEPROM RT configs to SRAM
    RAPID_TRIGGER_STATUS = eeprom_read_byte((const uint8_t*)&default_settings.rt_status);
    RAPID_TRIGGER_THRESHOLD = eeprom_read_byte((const uint8_t*)&default_settings.rt_threshold);
    RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD = eeprom_read_byte((const uint8_t*)&default_settings.rt_sc_threshold);

    // Load EEPROM SnapTap configs to SRAM
    SNAPTAP_A_STATUS = eeprom_read_byte((const uint8_t*)&default_settings.snaptap_a_status);
    SNAPTAP_A_KEY1_COORDS = eeprom_read_byte((const uint8_t*)(&default_settings.snaptap_a_key1));
    SNAPTAP_A_KEY2_COORDS = eeprom_read_byte((const uint8_t*)(&default_settings.snaptap_a_key2));
    SNAPTAP_B_STATUS = eeprom_read_byte((const uint8_t*)&default_settings.snaptap_b_status);
    SNAPTAP_B_KEY1_COORDS = eeprom_read_byte((const uint8_t*)(&default_settings.snaptap_b_key1));
    SNAPTAP_B_KEY2_COORDS = eeprom_read_byte((const uint8_t*)(&default_settings.snaptap_b_key2));
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

void save_snaptap_a_to_eeprom(uint8_t snaptap_status, uint8_t key1, uint8_t key2) {
    eeprom_update_byte(
        (uint8_t*)(&default_settings.snaptap_a_status),
        snaptap_status
    );
    eeprom_update_byte(
        (uint8_t*)(&default_settings.snaptap_a_key1),
        key1
    );
    eeprom_update_byte(
        (uint8_t*)(&default_settings.snaptap_a_key2),
        key2
    );
}

void save_snaptap_b_to_eeprom(uint8_t snaptap_status, uint8_t key1, uint8_t key2) {
    eeprom_update_byte(
        (uint8_t*)(&default_settings.snaptap_b_status),
        snaptap_status
    );
    eeprom_update_byte(
        (uint8_t*)(&default_settings.snaptap_b_key1),
        key1
    );
    eeprom_update_byte(
        (uint8_t*)(&default_settings.snaptap_b_key2),
        key2
    );
}

void save_rapid_trigger_to_eeprom(uint8_t rt_status, uint8_t rt_threshold, uint8_t rt_sc_threshold) {
    eeprom_update_byte(
        (uint8_t*)(&default_settings.rt_status),
        rt_status
    );
    eeprom_update_byte(
        (uint8_t*)(&default_settings.rt_threshold),
        rt_threshold
    );
    eeprom_update_byte(
        (uint8_t*)(&default_settings.rt_sc_threshold),
        rt_sc_threshold
    );
}

void save_rotary_encoder_to_eeprom(uint8_t ctclkw, uint8_t clkw, uint8_t pb) {
    eeprom_update_byte(
        (uint8_t*)(&default_settings.rotary_counterclockwise),
        ctclkw
    );
    eeprom_update_byte(
        (uint8_t*)(&default_settings.rotary_clockwise),
        clkw
    );
    eeprom_update_byte(
        (uint8_t*)(&default_settings.rotary_pushbutton),
        pb
    );
}