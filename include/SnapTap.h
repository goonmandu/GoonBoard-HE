#ifndef SNAPTAP_H
#define SNAPTAP_H


#include <stdint.h>

#define ROWOF(KEYCOORD) ((KEYCOORD & 0xF0) >> 4)
#define KEYOF(KEYCOORD) (KEYCOORD & 0x0F)
#define SNAPTAP_MODULE_A 1
#define SNAPTAP_MODULE_B 2

uint8_t snaptap(uint8_t snaptap_module, uint8_t** ptr_to_restore);


#endif /* SNAPTAP_H */