#ifndef JGOTTULA_M68K_MEM_H
#define JGOTTULA_M68K_MEM_H

#include <stdbool.h>
#include <stdint.h>

uint32_t memRead(uint32_t addr, uint8_t size);
void memWrite(uint32_t addr, uint8_t size, uint32_t value);

#endif
