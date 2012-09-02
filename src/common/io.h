#ifndef JGOTTULA_COMMON_IO_H
#define JGOTTULA_COMMON_IO_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

typedef volatile uint8_t *reg_t;
typedef volatile uint16_t *reg16_t;


/* WARNING: these write functions may need to be wrapped in ATOMIC_BLOCK()
 * because of their read/modify/write nature and the possibility that an ISR
 * could be RMW'ing the same register */

inline uint8_t readIO(reg_t reg, uint8_t mask)
{
	return *reg & mask;
}

inline void writeIO(reg_t reg, uint8_t mask, uint8_t val)
{
	if (mask == 0xff)
		*reg = val;
	else
		*reg = readIO(reg, ~mask) | (val & mask);
}

inline uint16_t readIO16(reg16_t reg, uint16_t mask)
{
	return *reg & mask;
}

inline void writeIO16(reg16_t reg, uint16_t mask, uint16_t val)
{
	if (mask == 0xffff)
		*reg = val;
	else
		*reg = readIO16(reg, ~mask) | (val & mask);
}

#endif
