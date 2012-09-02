#ifndef JGOTTULA_MCU_M68K_EA_H
#define JGOTTULA_MCU_M68K_EA_H

#include <stdbool.h>
#include <stdint.h>

uint32_t accessEA(const uint8_t *ptr, uint32_t addr, uint8_t mode, uint8_t reg,
	uint32_t data, uint8_t size, bool write);
uint8_t calcEA(const uint8_t *ptr, uint8_t mode, uint8_t reg, uint8_t size,
	uint32_t *addrOut);

#endif