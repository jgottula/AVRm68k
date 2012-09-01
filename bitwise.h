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

/* hack: the C spec says that whether right shifts on signed integers are
 * arithmetic or logical shifts is implementation-defined; we rely on the former
 * behavior for these functions, so we check for it
 * 
 * potential alternate approaches include writing the routine in assembly, or
 * doing it horrendously slowly in software */
_Static_assert(((int8_t)0x80 >> 1) == (int8_t)0xc0,
	"compiler discards MSB on signed right shifts!");

inline uint8_t shiftRightArithByte(uint8_t *operand, uint8_t shifts)
{
	int8_t signedOp = (int8_t)(*operand);
	uint8_t shifted = (uint8_t)(signedOp >> shifts);
	
	return (*operand = shifted);
}

inline uint16_t shiftRightArithWord(uint16_t *operand, uint8_t shifts)
{
	int16_t signedOp = (int16_t)(*operand);
	uint16_t shifted = (uint16_t)(signedOp >> shifts);
	
	return (*operand = shifted);
}

inline uint32_t shiftRightArithLong(uint32_t *operand, uint8_t shifts)
{
	int32_t signedOp = (int32_t)(*operand);
	uint32_t shifted = (uint32_t)(signedOp >> shifts);
	
	return (*operand = shifted);
}

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
