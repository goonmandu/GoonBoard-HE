#ifndef DEBUG_H
#define DEBUG_H


#ifdef LOGIC_ANALYZER_DEBUG
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


#endif /* DEBUG_H */