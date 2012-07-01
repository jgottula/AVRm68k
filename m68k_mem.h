#ifndef JGOTTULA_M68K_MEM_H
#define JGOTTULA_M68K_MEM_H

#include <stdbool.h>
#include <stdint.h>

uint8_t memReadBYTE(uint32_t addr);
uint16_t memReadWORD(uint32_t addr);
uint32_t memReadDWORD(uint32_t addr);
void memWriteBYTE(uint32_t addr, uint8_t value);
void memWriteWORD(uint32_t addr, uint16_t value);
void memWriteDWORD(uint32_t addr, uint32_t value);

#endif
