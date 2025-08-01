#ifndef SETTINGSEDITOR_H
#define SETTINGSEDITOR_H


#include "KeyboardConfig.h"

void load_eeprom_to_sram(void);
void save_keymap_to_eeprom(uint8_t row, uint8_t key, uint8_t keycode);
void save_actuation_to_eeprom(uint8_t row, uint8_t key, uint8_t actuation);
void save_rt_threshold_to_eeprom(uint8_t rt_threshold);
void save_rt_sc_threshold_to_eeprom(uint8_t rt_sc_threshold);
void save_snaptap_a_to_eeprom(uint8_t snaptap_status, uint8_t key1, uint8_t key2);
void save_snaptap_b_to_eeprom(uint8_t snaptap_status, uint8_t key1, uint8_t key2);


#endif /* SETTINGSEDITOR_H */