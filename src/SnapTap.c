#include "SnapTap.h"
#include "KeyboardConfig.h"
#include "MatrixScanner.h"

/**
 * To be called immediately after finalized Keyboard Report generation.
 * 
 * This is the specific SOCD variety: Key-Agnostic Last-In aka "SnapTap" by Wooting.
 * Let's say we have them mapped to A and D.
 * 1.   Idle.
 * 2.   A is pressed:
 *          register A as PRESSED (e.g. do nothing).
 * 3.   While A is held down, D is pressed:
 *          register A as RELEASED (update key_status) and register key 2 as PRESSED (do nothing).
 * 4.   If D is released:
 *          register A as PRESSED (do nothing)
 * 5.   If A is released:
 *          (do nothing)
 * 6.   Repeat
 * 
 * This also returns the pointer to the `key_status` matrix to restore the overridden key
 * for Rapid Trigger compatibility issues as Rapid Trigger assumes `key_status` doesn't change
 * outside of `MatrixScanner::scan_keys`.
 * 
 * If SnapTap didn't trigger, it returns 0xFFFF.
 */
uint8_t snaptap(uint8_t** ptr_to_restore) {
    static uint8_t first_key;

    // SnapTap initialization, record first keypress
    if (PRESSED == key_status[ROWOF(SNAPTAP_KEY1_COORDS)][KEYOF(SNAPTAP_KEY1_COORDS)]
        && RELEASED == key_status[ROWOF(SNAPTAP_KEY2_COORDS)][KEYOF(SNAPTAP_KEY2_COORDS)]) {
        first_key = SNAPTAP_KEY1_COORDS;
        return 0;
    }
    else if (RELEASED == key_status[ROWOF(SNAPTAP_KEY1_COORDS)][KEYOF(SNAPTAP_KEY1_COORDS)]
             && PRESSED == key_status[ROWOF(SNAPTAP_KEY2_COORDS)][KEYOF(SNAPTAP_KEY2_COORDS)]) {
        first_key = SNAPTAP_KEY2_COORDS;
        return 0;
    }

    // If both are now pressed down, cancel the first keypress
    else if (PRESSED == key_status[ROWOF(SNAPTAP_KEY1_COORDS)][KEYOF(SNAPTAP_KEY1_COORDS)]
             && PRESSED == key_status[ROWOF(SNAPTAP_KEY2_COORDS)][KEYOF(SNAPTAP_KEY2_COORDS)]) {
        // Simultaneous press: SnapTap does nothing
        if (first_key == 0xFF) return 0;
        key_status[ROWOF(first_key)][KEYOF(first_key)] = 0;
        *ptr_to_restore = (uint8_t*)&key_status[ROWOF(first_key)][KEYOF(first_key)];
        return 1;
    }
    // This branch is to mark first_key as stale the USB frame immediately after
    // both keys were released so that SnapTap doesn't filter out simlutaneous
    // presses randomly
    else {
        first_key = 0xFF;
        return 0;
    }
}