#include "Hacks.h"
#include "SnapTap.h"
#include "MatrixScanner.h"
#include "KeyboardConfig.h"

uint8_t values_buf_0[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t values_buf_1[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t adc_maxima[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t adc_minima[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t key_actions[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t key_status[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t key_idle_for[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t key_being_pressed_for[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t key_being_released_for[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t candidate_maxima[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t candidate_minima[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t ADC_BASELINE[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];

uint8_t (*adc_values)[MAX_KEYS_SUPPORTED_PER_ROW] = values_buf_0;
uint8_t (*old_values)[MAX_KEYS_SUPPORTED_PER_ROW] = values_buf_1;

void setup_mux_pins(void) {
    DDRD |= 0x1F;  // Row select Mux: PD3-0
    DDRF |= 0xF0;  // Key select Mux: PF7-4
}

void setup_led_pins(void) {
    DDRE |= (1 << PE6);
    DDRB |= (1 << PB4) | (1 << PB5);
    PORTE &= ~(1 << PE6);
    PORTB &= ~(1 << PB4 | 1 << PB5);
}

void capslock_on(void) {
    PORTE |= (1 << PE6);
}

void capslock_off(void) {
    PORTE &= ~(1 << PE6);
}

void numlock_on(void) {
    PORTB |= (1 << PB4);
}

void numlock_off(void) {
    PORTB &= ~(1 << PB4);
}

void scrlock_on(void) {
    PORTB |= (1 << PB5);
}

void scrlock_off(void) {
    PORTB &= ~(1 << PB5);
}

void scan_keys() {
    SET_DEBUG_BIT;
    register uint8_t spi_hi asm("r18");
    register uint8_t spi_lo asm("r19");

    // Swap two buffers to avoid byte-by-byte copy
    uint8_t (*tmp)[MAX_KEYS_SUPPORTED_PER_ROW] = adc_values;
    adc_values = old_values;
    old_values = tmp;

    uint8_t row_idx = 0;
    uint8_t key_idx = 0;
    PORTD = row_idx | (1 << PD5);
    PORTF = key_idx << 4;

    // Mux t_pd
    // ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP;

    while (row_idx < NUM_ROWS) {
        while (key_idx < NUM_KEYS_PER_ROW[row_idx]) {
            // Read ADC
            spi_select_slave();
            spi_hi = spi_read();
            spi_lo = spi_read();
            spi_deselect_slave();
            asm volatile(
                "push   r16         \n\t"
                "in     r16,    %2  \n\t"
                "push   r16         \n\t"
                "rol    %1          \n\t"
                "rol    %0          \n\t"
                "pop    r16         \n\t"
                "out    %2,     r16 \n\t"
                "pop    r16         \n\t"
                : "+r"(spi_hi), "+r"(spi_lo)
                : "I"(_SFR_IO_ADDR(SREG))
                : "r16", "cc"
            );

            // By incrementing keys mux between ADC read and doing processing,
            // we can offset Mux t_pd with processing time.
            PORTF = (key_idx + 1) << 4;
            adc_values[row_idx][key_idx] = spi_hi;
            
            // Filter unconnected keys
            // ADC halfscale = 0x7F at B = 0
            // Entire codebase assumes negative polarity switches
            if (spi_hi > 0x7F) {
                // Mux t_pd
                // ASM_NOP; ASM_NOP;
                key_idx++;
                continue;
            }
            
            // Only execute rapid trigger part when enabled
            if (!RAPID_TRIGGER_ENABLED) {
                if (ADC_BASELINE[row_idx][key_idx] - adc_values[row_idx][key_idx] >= ACTUATIONS_MATRIX[row_idx][key_idx]) {
                    key_status[row_idx][key_idx] = PRESSED;
                }
                else {
                    key_status[row_idx][key_idx] = RELEASED;
                }

                // Mux t_pd
                // ASM_NOP; ASM_NOP;
            }
            else {
                // Rapid Trigger!
                // If key is idle, skip and move to next key
                if (adc_values[row_idx][key_idx] == old_values[row_idx][key_idx]) {
                    key_idle_for[row_idx][key_idx]++;
                    if (key_actions[row_idx][key_idx] != IDLE \
                            && key_idle_for[row_idx][key_idx] >= RAPID_TRIGGER_IDLE_HYSTERESIS) {
                        key_idle_for[row_idx][key_idx] = 0;
                        key_actions[row_idx][key_idx] = IDLE;
                        adc_maxima[row_idx][key_idx] = spi_hi;
                        adc_minima[row_idx][key_idx] = spi_hi;
                    }
                }

                // If key is being pressed, record it
                else if (adc_values[row_idx][key_idx] < old_values[row_idx][key_idx]) {
                    key_idle_for[row_idx][key_idx] = 0;
                    if (key_being_pressed_for[row_idx][key_idx] == 0) {
                        candidate_maxima[row_idx][key_idx] = old_values[row_idx][key_idx];
                    }
                    key_being_pressed_for[row_idx][key_idx]++;
                    // If key is being pressed just now, record previous value as max
                    // to compare against rapid trigger threshold
                    if (key_actions[row_idx][key_idx] != BEING_PRESSED \
                        && (key_being_pressed_for[row_idx][key_idx] >= RAPID_TRIGGER_DIRECTION_HYSTERESIS \
                            || old_values[row_idx][key_idx] - adc_values[row_idx][key_idx] > RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD)) {
                        key_being_released_for[row_idx][key_idx] = 0;
                        key_actions[row_idx][key_idx] = BEING_PRESSED;
                        adc_maxima[row_idx][key_idx] = candidate_maxima[row_idx][key_idx];
                    }
                    // If key was already being pressed, send press keystroke if
                    // delta(current, highest) >= threshold
                    if (adc_maxima[row_idx][key_idx] - spi_hi >= RAPID_TRIGGER_THRESHOLD) {
                        key_status[row_idx][key_idx] = PRESSED;
                    }
                }

                // If key is being released, record it
                else {
                    key_idle_for[row_idx][key_idx] = 0;
                    if (key_being_released_for[row_idx][key_idx] == 0) {
                        candidate_minima[row_idx][key_idx] = old_values[row_idx][key_idx];
                    }
                    key_being_released_for[row_idx][key_idx]++;
                    // If key is being released just now, record previous value as min
                    // to compare against rapid trigger threshold
                    if (key_actions[row_idx][key_idx] != BEING_RELEASED \
                        && (key_being_released_for[row_idx][key_idx] >= RAPID_TRIGGER_DIRECTION_HYSTERESIS \
                            || adc_values[row_idx][key_idx] - old_values[row_idx][key_idx] > RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD)) {
                        key_being_pressed_for[row_idx][key_idx] = 0;
                        key_actions[row_idx][key_idx] = BEING_RELEASED;
                        adc_minima[row_idx][key_idx] = candidate_minima[row_idx][key_idx];
                    }
                    // If key was already being released, send release keystroke if
                    // delta(current, lowest) >= threshold
                    if (spi_hi - adc_minima[row_idx][key_idx] >= RAPID_TRIGGER_THRESHOLD \
                        || spi_hi >= ADC_BASELINE[row_idx][key_idx] - RAPID_TRIGGER_THRESHOLD) {
                        key_status[row_idx][key_idx] = RELEASED;
                    }
                }
                
                // Mux t_pd
                // ASM_NOP; ASM_NOP;
            }
            key_idx++;
        }
        key_idx = 0;
        row_idx++;
        PORTF = key_idx << 4;
        PORTD = row_idx | (1 << PD5);

        // Mux t_pd
        // ASM_NOP; ASM_NOP;
    }
    CLEAR_DEBUG_BIT;
}


// Call this after SPI initialization
// but before Global_Interrupt_Enable
// in the setup portion.
void measure_adc_baseline() {
    // ADS7885 Power-up sequence
    spi_select_slave();
    spi_read();
    spi_read();
    spi_deselect_slave();

    // Datasheet specifies max 0.8 usec between
    // Power-up signal to being ready for data
    _delay_us(1);

    uint8_t row_idx = 0;
    uint8_t key_idx = 0;
    PORTD = row_idx | (1 << PD5);
    PORTF = key_idx << 4;

    // 625 ns for t_pd
    ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP;

    while (row_idx < NUM_ROWS) {
        while (key_idx < MAX_KEYS_SUPPORTED_PER_ROW) {
            // Read ADC
            spi_select_slave();
            ADC_BASELINE[row_idx][key_idx] = (spi_read() << 1) | (spi_read() == 0x80);
            spi_deselect_slave();
            
            // By incrementing keys mux between ADC read and doing processing,
            // we can offset Mux t_pd with procesing time.
            key_idx++;
            PORTF = key_idx << 4;
            
            // Make GODDAMN sure t_pd has passed
            ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP; ASM_NOP;
        }
        key_idx = 0;
        row_idx++;
        PORTD = row_idx | (1 << PD5);
        PORTF = key_idx << 4;
    }
}