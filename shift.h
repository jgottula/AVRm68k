#ifndef JGOTTULA_SHIFT_H
#define JGOTTULA_SHIFT_H

#include <stdbool.h>
#include <stdint.h>

void shiftOut8(uint8_t byte);
void shiftOut16(uint16_t word);
void shiftOut24(uint32_t dword);
void shiftOut32(uint32_t dword);
void shiftZero(void);
void shiftInit(void);

#endif
