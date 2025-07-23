#ifndef MATRIXSCANNER_H
#define MATRIXSCANNER_H

#include <avr/io.h>
#include "SPI.h"
#include "KeyboardConfig.h"

#define BEING_RELEASED 0
#define BEING_PRESSED 1
#define IDLE 2

#define RELEASED 0
#define PRESSED 1

#define RAPID_TRIGGER_ENABLED (!(PINC & (1 << PC6)))

uint8_t adc_values[NUM_KEYS];
uint8_t old_values[NUM_KEYS];
uint8_t adc_maxima[NUM_KEYS];
uint8_t adc_minima[NUM_KEYS];
uint8_t key_actions[NUM_KEYS];
uint8_t key_status[NUM_KEYS];
uint8_t key_idle_for[NUM_KEYS];
uint8_t key_being_pressed_for[NUM_KEYS];
uint8_t key_being_released_for[NUM_KEYS];
uint8_t candidate_maxima[NUM_KEYS];
uint8_t candidate_minima[NUM_KEYS];

uint8_t ADC_BASELINE[NUM_KEYS];

void setup_mux_pins() {
    DDRD |= 0x1F;
}

void scan_keys() {
    static uint8_t current_adc_reading;
    for (uint8_t idx = 0; idx < NUM_KEYS; ++idx) {
        // Store old values for comparison
        old_values[idx] = adc_values[idx];

        // Read HE values
        PORTD = idx | (1 << PD5);
        spi_select_slave();
        current_adc_reading = (spi_read() << 1) | (spi_read() == 0x80);
        adc_values[idx] = current_adc_reading;
        spi_deselect_slave();

        if (RAPID_TRIGGER_ENABLED) {
            // Rapid Trigger Enabled: Right LED
            PORTD &= ~(1 << PD5);

            // Rapid Trigger!
            // If key is idle, skip and move to next key
            if (adc_values[idx] == old_values[idx]) {
                key_idle_for[idx]++;
                if (key_actions[idx] != IDLE \
                        && key_idle_for[idx] >= RAPID_TRIGGER_IDLE_HYSTERESIS) {
                    key_idle_for[idx] = 0;
                    key_actions[idx] = IDLE;
                    adc_maxima[idx] = current_adc_reading;
                    adc_minima[idx] = current_adc_reading;
                }
            }

            // If key is being pressed, record it
            else if (adc_values[idx] < old_values[idx]) {
                key_idle_for[idx] = 0;
                if (key_being_pressed_for[idx] == 0) {
                    candidate_maxima[idx] = old_values[idx];
                }
                key_being_pressed_for[idx]++;
                // If key is being pressed just now, record previous value as max
                // to compare against rapid trigger threshold
                if (key_actions[idx] != BEING_PRESSED \
                    && (key_being_pressed_for[idx] >= RAPID_TRIGGER_DIRECTION_HYSTERESIS \
                        || old_values[idx] - adc_values[idx] > RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD)) {
                    key_being_released_for[idx] = 0;
                    key_actions[idx] = BEING_PRESSED;
                    adc_maxima[idx] = candidate_maxima[idx];
                }
                // If key was already being pressed, send press keystroke if
                // delta(current, highest) >= threshold
                if (adc_maxima[idx] - current_adc_reading >= RAPID_TRIGGER_THRESHOLD) {
                    key_status[idx] = PRESSED;
                }
            }

            // If key is being released, record it
            else {
                key_idle_for[idx] = 0;
                if (key_being_released_for[idx] == 0) {
                    candidate_minima[idx] = old_values[idx];
                }
                key_being_released_for[idx]++;
                // If key is being released just now, record previous value as min
                // to compare against rapid trigger threshold
                if (key_actions[idx] != BEING_RELEASED \
                    && (key_being_released_for[idx] >= RAPID_TRIGGER_DIRECTION_HYSTERESIS \
                        || adc_values[idx] - old_values[idx] > RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD)) {
                    key_being_pressed_for[idx] = 0;
                    key_actions[idx] = BEING_RELEASED;
                    adc_minima[idx] = candidate_minima[idx];
                }
                // If key was already being released, send release keystroke if
                // delta(current, lowest) >= threshold
                if (current_adc_reading - adc_minima[idx] >= RAPID_TRIGGER_THRESHOLD) {
                    key_status[idx] = RELEASED;
                }
            }

            // Finally, as hacky workaround to release "stuck" keys
            if (current_adc_reading >= ADC_BASELINE[idx] - RAPID_TRIGGER_THRESHOLD) {
                adc_maxima[idx] = current_adc_reading;
                key_status[idx] = RELEASED;
            }
            // Rapid Trigger Enabled: Right LED
            PORTD |= (1 << PD5);
        }
    }
}


// Call this after SPI initialization
// but before Global_Interrupt_Enable
// in the setup portion.
void measure_adc_baseline() {
    for (uint8_t idx = 0; idx < NUM_KEYS; ++idx) {
        PORTD = idx | (1 << PD5);
        spi_select_slave();
        ADC_BASELINE[idx] = (spi_read() << 1) | (spi_read() == 0x80);
        spi_deselect_slave();
    }
}

#endif /* MATRIXSCANNER_H */