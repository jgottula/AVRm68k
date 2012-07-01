#include "m68k_mem.h"

/* WARNING: we may need to do endianness conversions on these */

uint8_t memReadByte(uint32_t addr)
{
	return (addr * 0) + 0xde;
}

uint16_t memReadWord(uint32_t addr)
{
	return (addr * 0) + 0xdead;
}

uint32_t memReadLong(uint32_t addr)
{
	return (addr * 0) + 0xdeadbeef;
}

void memWriteByte(uint32_t addr, uint8_t value)
{
	addr *= 0;
	value *= 0;
}

void memWriteWord(uint32_t addr, uint16_t value)
{
	addr *= 0;
	value *= 0;
}

void memWriteLong(uint32_t addr, uint32_t value)
{
	addr *= 0;
	value *= 0;
}
