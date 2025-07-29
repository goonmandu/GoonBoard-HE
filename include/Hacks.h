#ifndef HACKS_H
#define HACKS_H


#ifdef LA_OUTPUT
    // PD7 is Logic Analyzer Debug Bit
    #define DEBUG_BIT_PIN_CONFIG do { \
        DDRD |= (1 << PD7);           \
    } while (0)

    #define SET_DEBUG_BIT do {        \
        PORTD |= (1 << PD7);          \
    } while (0)

    #define CLEAR_DEBUG_BIT do {      \
        PORTD &= ~(1 << PD7);         \
    } while (0)
#else
    #define DEBUG_BIT_PIN_CONFIG
    #define SET_DEBUG_BIT
    #define CLEAR_DEBUG_BIT
#endif /* LA_OUTPUT */

#define ASM_NOP asm volatile ("nop\n\t")


#endif /* HACKS_H */