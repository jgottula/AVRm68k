#ifndef JGOTTULA_M68K_EA_H
#define JGOTTULA_M68K_EA_H

#include <stdbool.h>
#include <stdint.h>

uint32_t accessEA(uint32_t addr, uint8_t mode, uint8_t reg, uint32_t data,
	uint8_t size, bool write);
uint32_t calcEA(uint8_t mode, uint8_t reg);

#endif