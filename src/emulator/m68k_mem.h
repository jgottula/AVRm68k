#ifndef JGOTTULA_EMULATOR_M68K_MEM_H
#define JGOTTULA_EMULATOR_M68K_MEM_H

#include <stdbool.h>
#include <stdint.h>

uint32_t memRead(uint32_t addr, uint8_t size);
void memWrite(uint32_t addr, uint8_t size, uint32_t value);
void memDump(uint32_t addr, uint16_t len);

#endif
