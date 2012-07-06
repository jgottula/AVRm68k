#include "m68k_instr.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_mem.h"
#include "uart.h"

extern uint8_t *instr;

static uint16_t decodeBigEndian16(const uint8_t *bytes)
{
	return ((uint16_t)bytes[0] << 8) | bytes[1];
}

static uint32_t decodeBigEndian32(const uint8_t *bytes)
{
	return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8) | bytes[3];
}

static uint16_t signExtend8to16(uint8_t byte)
{
	if ((int8_t)byte < 0)
		return 0xff00 | byte;
	else
		return byte;
}

static uint32_t signExtend8to32(uint8_t byte)
{
	if ((int8_t)byte < 0)
		return 0xffffff00 | byte;
	else
		return byte;
}

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

static bool getExtType()
{
	const uint8_t *extWord = instr + 2;
	
	if (extWord[0] & 0b00000001)
		return EXT_FULL;
	else
		return EXT_BRIEF;
}

static uint32_t accessEA(uint32_t addr, uint8_t mode, uint8_t reg,
	uint32_t data, uint8_t size, bool write)
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

static uint32_t calcEA(uint8_t mode, uint8_t reg)
{
	uint32_t effAddr;
	
	switch (mode)
	{
	case AMODE_DREGDIRECT:
	case AMODE_AREGDIRECT:
		effAddr = 0;
		break;
	case AMODE_AREGINDIRECT:
	case AMODE_AREGPOSTINC:
	case AMODE_AREGPREDEC:
		effAddr = cpu.ureg.a[reg].l;
		break;
	case AMODE_AREGDISPLACE:
		assert(0);
	case AMODE_AREGINDEXED:
		assert(0);
	case AMODE_EXTRA:
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
	default:
		assert(0);
	}
	
	return effAddr;
}

bool instrEmu(void)
{
	uint16_t instrWord = decodeBigEndian16(instr);
	
	switch (instrWord)
	{
	case EMUINSTR_DUMPREG:
		uartWritePSTR("emu: dumpreg\n");
		m68kDumpReg();
		cpu.ureg.pc.l += 2;
		return true;
	case EMUINSTR_DUMPMEM:
		uartWritePSTR("emu: dumpmem\n");
		uint32_t addr = decodeBigEndian32(instr + 2);
		uint16_t lines = decodeBigEndian16(instr + 6);
		memDump(addr, lines);
		cpu.ureg.pc.l += 8;
		return true;
	default:
		return false;
	}
}

void instrNop(void)
{
	/* debug */
	uartWritePSTR("nop\n");
	
	cpu.ureg.pc.l += 2;
	
	/* does not affect CCR */
}

/* untested */
void instrExg(void)
{
	/* debug */
	uartWritePSTR("exg\n");
	
	cpu.ureg.pc.l += 2;
	
	uint8_t regIdx1 = (instr[0] & 0b00001110) >> 1;
	uint8_t regIdx2 = (instr[1] & 0b00000111);
	
	Reg *reg1, *reg2;
	
	switch ((instr[1] & 0b11111000) >> 3)
	{
	case 0b01000:
		reg1 = &cpu.ureg.d[regIdx1];
		reg2 = &cpu.ureg.d[regIdx2];
		break;
	case 0b01001:
		reg1 = &cpu.ureg.a[regIdx1];
		reg2 = &cpu.ureg.a[regIdx2];
		break;
	case 0b10001:
		reg1 = &cpu.ureg.d[regIdx1];
		reg2 = &cpu.ureg.a[regIdx2];
		break;
	default:
		assert(0);
	}
	
	Reg temp = *reg1;
	*reg1 = *reg2;
	*reg2 = temp;
	
	/* does not affect CCR */
}

/* untested */
void instrMoveFromCcr(void)
{
	/* debug */
	uartWritePSTR("move ccr,<ea>\n");
	
	cpu.ureg.pc.l += 2; // NOTE: may be larger for EA extensions
	
	/* get just the user portion of the SR */
	uint16_t ccr = cpu.ureg.sr.l & (SR_CARRY | SR_OVERFLOW | SR_ZERO |
		SR_NEGATIVE | SR_EXTEND);
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr = calcEA(mode, reg);
	accessEA(effAddr, mode, reg, ccr, SIZE_WORD, true);
	
	/* does not affect CCR */
}

/* untested */
void instrMoveq(void)
{
	/* debug */
	uartWritePSTR("moveq\n");
	
	cpu.ureg.pc.l += 2;
	
	uint8_t reg = (instr[0] & 0b1110) >> 1;
	uint8_t data = instr[1];
	
	cpu.ureg.d[reg].l = signExtend8to32(instr[1]);
	
	/* update condition codes */
	cpu.ureg.sr.l &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if ((int8_t)data == 0)
		cpu.ureg.sr.l |= SR_ZERO;
	else if ((int8_t)data < 0)
		cpu.ureg.sr.l |= SR_NEGATIVE;
}

/* untested */
void instrClr(void)
{
	/* debug */
	uartWritePSTR("clr\n");
	
	cpu.ureg.pc.l += 2; // NOTE: may be larger for EA extensions
	
	uint8_t size = instr[1] >> 6;
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr = calcEA(mode, reg);
	accessEA(effAddr, mode, reg, 0, size, true);
	
	cpu.ureg.sr.l &= ~(SR_CARRY | SR_OVERFLOW | SR_NEGATIVE);
	cpu.ureg.sr.l |= SR_ZERO;
}
