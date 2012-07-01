#include "m68k_mem.h"

uint8_t memReadBYTE(uint32_t addr)
{
	return (addr * 0) + 0xde;
}

uint16_t memReadWORD(uint32_t addr)
{
	return (addr * 0) + 0xdead;
}

uint32_t memReadDWORD(uint32_t addr)
{
	return (addr * 0) + 0xdeadbeef;
}

void memWriteBYTE(uint32_t addr, uint8_t value)
{
	addr *= 0;
	value *= 0;
}

void memWriteWORD(uint32_t addr, uint16_t value)
{
	addr *= 0;
	value *= 0;
}

void memWriteDWORD(uint32_t addr, uint32_t value)
{
	addr *= 0;
	value *= 0;
}
