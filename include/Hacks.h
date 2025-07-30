#ifndef HACKS_H
#define HACKS_H


#ifdef DEBUG_BITS
    // PD7 is Logic Analyzer Debug Bit
    #define DEBUG_BIT_PIN_CONFIG do { DDRD |= (1 << PD7); } while (0)
    #define SET_DEBUG_BIT do { PORTD |= (1 << PD7); } while (0)
    #define CLEAR_DEBUG_BIT do { PORTD &= ~(1 << PD7); } while (0)

    #define SET_LEFT_LED do { PORTB &= ~(1 << PB0); } while (0)
    #define CLEAR_LEFT_LED do { PORTB |= (1 << PB0); } while (0)
    #define SET_RIGHT_LED do { PORTD &= ~(1 << PD5); } while (0)
    #define CLEAR_RIGHT_LED do { PORTD |= (1 << PD5); } while (0)
#else
    #define DEBUG_BIT_PIN_CONFIG
    #define SET_DEBUG_BIT
    #define CLEAR_DEBUG_BIT

    #define SET_LEFT_LED
    #define CLEAR_LEFT_LED
    #define SET_RIGHT_LED
    #define CLEAR_RIGHT_LED
#endif /* DEUBG_BITS */

#define ASM_NOP asm volatile ("nop\n\t")


#endif /* HACKS_H */