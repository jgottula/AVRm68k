#include "m68k_ea.h"
#include "bitwise.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_instr.h"
#include "m68k_mem.h"

static uint32_t readReg(uint8_t mode, uint8_t reg, int8_t size)
{
	Reg *src;
	
	switch (mode)
	{
	case AMODE_DREGDIRECT:
		src = &cpu.ureg.d[reg];
		break;
	case AMODE_AREGDIRECT:
		src = &cpu.ureg.a[reg];
		break;
	default:
		assert(0);
	}
	
	switch (size)
	{
	case SIZE_BYTE:
		return src->b[0];
	case SIZE_WORD:
		return src->w[0];
	case SIZE_LONG:
		return src->l;
	default:
		assert(0);
	}
}

static void writeReg(uint8_t mode, uint8_t reg, uint32_t data, int8_t size)
{
	Reg *dest;
	
	switch (mode)
	{
	case AMODE_DREGDIRECT:
		dest = &cpu.ureg.d[reg];
		break;
	case AMODE_AREGDIRECT:
		dest = &cpu.ureg.a[reg];
		break;
	default:
		assert(0);
	}
	
	switch (size)
	{
	case SIZE_BYTE:
		dest->b[0] = (uint8_t)data;
		break;
	case SIZE_WORD:
		dest->w[0] = (uint16_t)data;
		break;
	case SIZE_LONG:
		dest->l = (uint32_t)data;
		break;
	default:
		assert(0);
	}
}

static void prePostIncrDecr(bool incr, uint8_t reg, uint8_t size)
{
	uint8_t actualSize;
	
	switch (size)
	{
	case SIZE_BYTE:
		if (reg == 7) // special case for stack pointer
			actualSize = 2;
		else
			actualSize = 1;
		break;
	case SIZE_WORD:
		actualSize = 2;
		break;
	case SIZE_LONG:
		actualSize = 4;
		break;
	default:
		assert(0);
	}
	
	if (incr)
		cpu.ureg.a[reg].l += actualSize;
	else
		cpu.ureg.a[reg].l -= actualSize;
}

uint32_t accessEA(uint32_t addr, uint8_t mode, uint8_t reg, uint32_t data,
	uint8_t size, bool write)
{
	uint32_t rtn = 0;
	
	if (mode == AMODE_AREGPREDEC)
		prePostIncrDecr(false, reg, size);
	
	switch (mode)
	{
	case AMODE_DREGDIRECT:
	case AMODE_AREGDIRECT:
		if (write)
			writeReg(mode, reg, data, size);
		else
			rtn = readReg(mode, reg, size);
		break;
	default:
		if (write)
			memWrite(addr, size, data);
		else
			rtn = memRead(addr, size);
		break;
	}
	
	if (mode == AMODE_AREGPOSTINC)
		prePostIncrDecr(true, reg, size);
	
	return rtn;
}

#warning calcEA: TEST!
uint8_t calcEA(uint8_t mode, uint8_t reg, uint32_t *addrOut)
{
	/* the function's return value is the number of bytes taken up by the EA
	 * extension words */
	
	switch (mode)
	{
	case AMODE_DREGDIRECT:
	case AMODE_AREGDIRECT:
		return 0;
	case AMODE_AREGINDIRECT:
	case AMODE_AREGPOSTINC:
	case AMODE_AREGPREDEC:
	{
		*addrOut = cpu.ureg.a[reg].l;
		return 0;
	}
	case AMODE_AREGDISPLACE:
	{
		uint16_t displacement = decodeBigEndian16(instr + 2);
		*addrOut = cpu.ureg.a[reg].l + signExtend16to32(displacement);
		
		return 2;
	}
	case AMODE_AREGINDEXED:
	{
		uint16_t briefExt = decodeBigEndian16(instr + 2);
		uint8_t displacement = (uint8_t)briefExt;
		uint8_t reg = ((briefExt >> 12) & 0b111);
		bool size = ((briefExt & (1 << 11)) == 1);
		uint8_t scale = ((briefExt >> 9) & 0b11);
		const Reg *idxReg;
		
		if (briefExt & (1 << 15))
			idxReg = &cpu.ureg.a[reg];
		else
			idxReg = &cpu.ureg.d[reg];
		
		uint32_t idxValue;
		
		if (size)
			idxValue = idxReg->l;
		else
			idxValue = signExtend16to32(idxReg->w[0]);
		
		idxValue <<= scale;
		
		*addrOut = cpu.ureg.a[reg].l + signExtend8to32(displacement) + idxValue;
		
		return 2;
	}
	case AMODE_EXTRA:
	{
		switch (reg)
		{
		case AMODE_EXTRA_ABSSHORT:
			assert(0);
		case AMODE_EXTRA_ABSLONG:
			assert(0);
		case AMODE_EXTRA_PCDISPLACE:
			assert(0);
		case AMODE_EXTRA_PCINDEXED:
			assert(0);
		case AMODE_EXTRA_IMMEDIATE:
			assert(0);
		default:
			assert(0);
		}
	}
	default:
		assert(0);
	}
}
