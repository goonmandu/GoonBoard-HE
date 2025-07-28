#ifndef MATRIX_SCANNER_H
#define MATRIX_SCANNER_H

#include <avr/io.h>
#include <stdint.h>
#include "SPI.h"
#include "KeyboardConfig.h"

#define BEING_RELEASED 0
#define BEING_PRESSED  1
#define IDLE           2

#define RELEASED 0
#define PRESSED  1

#define RAPID_TRIGGER_ENABLED (!(PINC & (1 << PC6)))

extern uint8_t adc_values[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t old_values[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t adc_maxima[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t adc_minima[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t key_actions[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t key_status[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t key_idle_for[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t key_being_pressed_for[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t key_being_released_for[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t candidate_maxima[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t candidate_minima[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
extern uint8_t ADC_BASELINE[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];

void setup_mux_pins(void);
void scan_keys(void);
void measure_adc_baseline(void);

#endif /* MATRIX_SCANNER_H */
