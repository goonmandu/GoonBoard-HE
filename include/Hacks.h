#ifndef HACKS_H
#define HACKS_H


#ifdef DEBUG
    // PE6 is Logic Analyzer Debug Bit
    #define DEBUG_BIT_PIN_CONFIG do { \
        DDRE |= (1 << PE6);           \
    } while (0)

    #define SET_DEBUG_BIT do {        \
        PORTE |= (1 << PE6);          \
    } while (0)

    #define CLEAR_DEBUG_BIT do {      \
        PORTE &= ~(1 << PE6);         \
    } while (0)
#else
    #define DEBUG_BIT_PIN_CONFIG
    #define SET_DEBUG_BIT
    #define CLEAR_DEBUG_BIT
#endif /* LOGIC_ANALYZER_DEBUG */

#define ASM_NOP asm volatile ("nop\n\t")


#endif /* HACKS_H */