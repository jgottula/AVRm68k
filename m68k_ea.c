#include "m68k_ea.h"
#include "bitwise.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_instr.h"
#include "m68k_mem.h"

static bool extWordType(const uint8_t *ptr)
{
	return ((*ptr & 0b00000001) != 0);
}

static uint32_t readReg(uint8_t mode, uint8_t reg, int8_t size)
{
	Reg32 *src;
	
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
	Reg32 *dest;
	
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

static uint32_t prePostIncrDecr(bool incr, uint8_t reg, uint8_t size)
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
	
	return cpu.ureg.a[reg].l;
}

static uint8_t calcBrief(const uint8_t *ptr, uint8_t reg, uint32_t *addrOut,
	bool pcRelative)
{
	uint8_t briefExtH = ptr[0], briefExtL = ptr[1];
	
	bool idxRegType = ((briefExtH & 0b10000000) != 0);
	uint8_t idxRegNum = (briefExtH >> 4) & 0b111;
	bool idxSize = ((briefExtH & (1 << 3)) != 0);
	uint8_t idxScale = ((briefExtH >> 1) & 0b11);
	uint8_t dispByte = briefExtL;
	
	uint32_t index, baseDisp;
	
	if (!idxRegType)
		index = cpu.ureg.d[idxRegNum].l;
	else
		index = cpu.ureg.a[idxRegNum].l;
	
	if (!idxSize)
		index = signExtend16to32(index);
	
	index <<= idxScale;
	baseDisp = signExtend8to32(dispByte);
	
	if (!pcRelative)
		*addrOut = cpu.ureg.a[reg].l + index + baseDisp;
	else
		*addrOut = cpu.ureg.pc.l + index + baseDisp;
	
	return 2;
}

static uint8_t calcFull(const uint8_t *ptr, uint8_t reg, uint32_t *addrOut,
	bool pcRelative)
{
	uint8_t extWords = 1;
	
	uint8_t fullExtH = ptr[0], fullExtL = ptr[1];
	
	bool idxRegType = ((fullExtH & 0b10000000) != 0);
	uint8_t idxRegNum = (fullExtH >> 4) & 0b111;
	bool idxSize = ((fullExtH & (1 << 3)) != 0);
	uint8_t idxScale = ((fullExtH >> 1) & 0b11);
	
	bool addrRegSuppress = ((fullExtL & 0b10000000) != 0);
	bool idxSuppress = ((fullExtL & 0b01000000) != 0);
	uint8_t baseSize = ((fullExtL >> 4) & 0b11);
	bool idxMode = ((fullExtL & 0b00000100) != 0);
	uint8_t outerSize = (fullExtL & 0b11);
	
	ptr += 2;
	
	uint32_t addrBase;
	
	if (!addrRegSuppress)
	{
		if (!pcRelative)
			addrBase = cpu.ureg.a[reg].l;
		else
			addrBase = cpu.ureg.pc.l;
	}
	else
		addrBase = 0;
	
	uint32_t index;
	
	if (!idxSuppress)
	{
		if (!idxRegType)
			index = cpu.ureg.d[idxRegNum].l;
		else
			index = cpu.ureg.a[idxRegNum].l;
		
		if (!idxSize)
			index = signExtend16to32(index);
		
		index <<= idxScale;
	}
	else
		index = 0;
	
	uint32_t baseDisp;
	
	switch (baseSize)
	{
	case BD_SIZE_NULL:
		baseDisp = 0;
		break;
	case BD_SIZE_WORD:
		baseDisp = signExtend16to32(decodeBigEndian16(ptr));
		ptr += 2;
		++extWords;
		break;
	case BD_SIZE_LONG:
		baseDisp = decodeBigEndian32(ptr);
		ptr += 4;
		extWords += 2;
		break;
	default:
		assert(0);
	}
	
	uint32_t outerDisp;
	
	bool noMemIndirect = false;
	
	switch (outerSize)
	{
	case BD_SIZE_NULL:
		outerDisp = 0;
		break;
	case BD_SIZE_WORD:
		outerDisp = signExtend16to32(decodeBigEndian16(ptr));
		ptr += 2;
		++extWords;
		break;
	case BD_SIZE_LONG:
		outerDisp = decodeBigEndian32(ptr);
		ptr += 4;
		extWords += 2;
		break;
	default:
		noMemIndirect = true;
		break;
	}
	
	if (noMemIndirect) // addr indirect with idx, base disp
		*addrOut = addrBase + index + baseDisp;
	else
	{
		if (!idxMode) // mem indirect preindexed
		{
			uint32_t intermAddr = addrBase + baseDisp + index;
			
			uint32_t intermVal = memRead(intermAddr, SIZE_LONG);
			
			*addrOut = intermVal + outerDisp;
		}
		else // mem indirect postindexed
		{
			uint32_t intermAddr = addrBase + baseDisp;
			
			uint32_t intermVal = memRead(intermAddr, SIZE_LONG);
			
			*addrOut = intermVal + index + outerDisp;
		}
	}
	
	return extWords * 2;
}

uint32_t accessEA(const uint8_t *ptr, uint32_t addr, uint8_t mode, uint8_t reg,
	uint32_t data, uint8_t size, bool write)
{
	uint32_t rtn = 0;
	
	if (mode == AMODE_AREGPREDEC)
		addr = prePostIncrDecr(false, reg, size);
	
	switch (mode)
	{
	case AMODE_DREGDIRECT:
	case AMODE_AREGDIRECT:
	{
		if (write)
			writeReg(mode, reg, data, size);
		else
			rtn = readReg(mode, reg, size);
		break;
	}
	case AMODE_EXTRA:
	{
		if (reg == AMODE_EXTRA_IMMEDIATE)
		{
			assert(!write);
			
			switch (size)
			{
			case SIZE_BYTE:
				return (uint8_t)decodeBigEndian16(ptr);
			case SIZE_WORD:
				return decodeBigEndian16(ptr);
			case SIZE_LONG:
				return decodeBigEndian32(ptr);
			default:
				assert(0);
			}
		}
		
		/* all other modes will fall through to default (for mem read/write) */
	}
	default:
	{
		if (write)
			memWrite(addr, size, data);
		else
			rtn = memRead(addr, size);
		break;
	}
	}
	
	if (mode == AMODE_AREGPOSTINC)
		addr = prePostIncrDecr(true, reg, size);
	
	return rtn;
}

uint8_t calcEA(const uint8_t *ptr, uint8_t mode, uint8_t reg, uint8_t size,
	uint32_t *addrOut)
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
		uint16_t displacement = decodeBigEndian16(ptr);
		*addrOut = cpu.ureg.a[reg].l + signExtend16to32(displacement);
		
		return 2;
	}
	case AMODE_INDEXED:
	{
		if (extWordType(ptr)) // full ext word
			return calcFull(ptr, reg, addrOut, false);
		else // brief ext word
			return calcBrief(ptr, reg, addrOut, false);
	}
	case AMODE_EXTRA:
	{
		switch (reg)
		{
		case AMODE_EXTRA_ABSSHORT:
		{
			*addrOut = signExtend16to32(decodeBigEndian16(ptr));
			return 2;
		}
		case AMODE_EXTRA_ABSLONG:
		{
			*addrOut = decodeBigEndian32(ptr);
			return 4;
		}
		case AMODE_EXTRA_PCDISPLACE:
		{
			uint16_t displacement = decodeBigEndian16(ptr);
			*addrOut = cpu.ureg.pc.l + signExtend16to32(displacement);
			
			return 2;
		}
		case AMODE_EXTRA_PCINDEXED:
		{
			if (extWordType(ptr)) // full ext word
				return calcFull(ptr, reg, addrOut, true);
			else // brief ext word
				return calcBrief(ptr, reg, addrOut, true);
		}
		case AMODE_EXTRA_IMMEDIATE:
		{
			switch (size)
			{
			case SIZE_BYTE:
			case SIZE_WORD:
				return 2;
			case SIZE_LONG:
				return 4;
			default:
				assert(0);
			}
		}
		default:
			assert(0);
		}
	}
	default:
		assert(0);
	}
}
