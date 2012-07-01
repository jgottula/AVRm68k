#ifndef JGOTTULA_M68K_MEM_H
#define JGOTTULA_M68K_MEM_H

#include <stdbool.h>
#include <stdint.h>

uint8_t memReadByte(uint32_t addr);
uint16_t memReadWord(uint32_t addr);
uint32_t memReadLong(uint32_t addr);
void memWriteByte(uint32_t addr, uint8_t value);
void memWriteWord(uint32_t addr, uint16_t value);
void memWriteLong(uint32_t addr, uint32_t value);

#endif
