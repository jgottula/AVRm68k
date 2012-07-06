#include "m68k_instr.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_mem.h"
#include "uart.h"

extern uint8_t *instr;
extern uint16_t *instrWord;

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

static void writeReg(uint8_t mode, uint8_t reg, uint32_t data, int8_t size)
{
	Reg *dest;
	
	switch (mode)
	{
	case 0b000:
		dest = &cpu.ureg.d[reg];
		break;
	case 0b001:
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

static void writeEA(uint8_t mode, uint8_t reg, uint32_t data, uint8_t size,
	bool signExtend)
{
	if (signExtend)
	{
		switch (size)
		{
		case SIZE_BYTE:
			if ((int8_t)data < 0)
				data |= 0xffffff00;
			else
				data &= 0xffffff00;
			break;
		case SIZE_WORD:
			if ((int16_t)data < 0)
				data |= 0xffff0000;
			else
				data &= 0xffff0000;
			break;
		default:
			assert(0);
		}
		
		/* write the entire long value now */
		size = SIZE_LONG;
	}
	
	switch (mode)
	{
	case 0b000: // data reg direct
	case 0b001: // addr reg direct
		writeReg(mode, reg, data, size);
		break;
	case 0b010: // addr reg indirect
		memWrite(cpu.ureg.a[reg].l, size, data);
		break;
	case 0b011: // addr reg indirect (post-increment)
		memWrite(cpu.ureg.a[reg].l, size, data);
		prePostIncrDecr(true, reg, size);
		break;
	case 0b100: // addr reg indirect (pre-decrement)
		prePostIncrDecr(false, reg, size);
		memWrite(cpu.ureg.a[reg].l, size, data);
		break;
	case 0b101: // addr reg indirect (displacement)
		assert(0);
	case 0b110: // multiple addr reg indirect
		assert(0);
	case 0b111: // multiple pc relative
		assert(0);
	default:
		assert(0);
	}
}

bool instrEmu(void)
{
	switch (instr[0] & 0b1111)
	{
	case 0b0000:
		uartWritePSTR("emu: dump reg\n");
		m68kDumpReg();
		break;
	default:
		return false;
	}
	
	cpu.ureg.pc.l += 1;
	
	return true;
}

void instrNop(void)
{
	/* debug */
	uartWritePSTR("nop\n");
	
	cpu.ureg.pc.l += 2;
	
	/* does not affect CCR */
}

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
	
	writeEA(mode, reg, ccr, SIZE_WORD, false);
	
	/* does not affect CCR */
}

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

void instrClr(void)
{
	/* debug */
	uartWritePSTR("clr\n");
	
	cpu.ureg.pc.l += 2; // NOTE: may be larger for EA extensions
	
	uint8_t size = instr[1] >> 6;
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	writeEA(mode, reg, 0, size, false);
	
	cpu.ureg.sr.l &= ~(SR_CARRY | SR_OVERFLOW | SR_NEGATIVE);
	cpu.ureg.sr.l |= SR_ZERO;
}
