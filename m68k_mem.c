#include "m68k_mem.h"
#include "debug.h"
#include "dram.h"
#include "m68k.h"

uint32_t memRead(uint32_t addr, uint8_t size)
{
	uint8_t bytes[4];
	
	bytes[0] = dramRead(addr);
	
	if (size == SIZE_BYTE)
		return bytes[0];
	
	bytes[1] = dramRead(addr + 1);
	
	if (size == SIZE_WORD)
		return ((uint16_t)bytes[0] << 8) | bytes[1];
	
	bytes[2] = dramRead(addr + 2);
	bytes[3] = dramRead(addr + 3);
	
	if (size == SIZE_LONG)
		return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
			((uint32_t)bytes[2] << 8) | bytes[3];
	
	assert(0);
}

void memWrite(uint32_t addr, uint8_t size, uint32_t value)
{
	switch (size)
	{
	case SIZE_BYTE:
		dramWrite(addr, value);
		break;
	case SIZE_WORD:
		dramWrite(addr, (value >> 8));
		dramWrite(addr + 1, value);
		break;
	case SIZE_LONG:
		dramWrite(addr, (value >> 24));
		dramWrite(addr + 1, (value >> 16));
		dramWrite(addr + 2, (value >> 8));
		dramWrite(addr + 3, value);
		break;
	default:
		assert(0);
	}
}
