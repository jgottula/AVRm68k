#ifndef JGOTTULA_BITWISE_H
#define JGOTTULA_BITWISE_H

#include <stdbool.h>
#include <stdint.h>
#include <avr/cpufunc.h>
#include <avr/io.h>

uint16_t decodeBigEndian16(const uint8_t *bytes);
uint32_t decodeBigEndian32(const uint8_t *bytes);
uint16_t signExtend8to16(uint8_t byte);
uint32_t signExtend8to32(uint8_t byte);
uint32_t signExtend16to32(uint16_t word);

inline uint8_t getStatusReg(void)
{
#warning "getStatusReg is horrible and needs to die"
	
	/* this is probably the worst possible way to get condition flags after a
	 * math operation: it totally depends on the last instruction before this
	 * function being the math instruction itself (which could depend on
	 * optimization level), and 16-/32-bit operations frequently involve
	 * multiple instructions, so it's questionable whether the last instruction
	 * will have the proper flags in these cases */
	
	/* ensure proper ordering of instructions with a memory barrier */
	asm volatile("" ::: "memory");
	
	return SREG;
}

#endif
