#include "m68k_instr.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_mem.h"
#include "uart.h"

extern uint8_t *instr;
extern uint16_t *instrWord;

static void writeReg(uint8_t mode, uint8_t reg, uint32_t data, int8_t size,
	bool signExtend)
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
	
	if (signExtend)
	{
		switch (size)
		{
		case SIZE_BYTE:
			if ((int8_t)data < 0)
				dest->l |= 0xffffff00;
			else
				dest->l &= 0xffffff00;
			break;
		case SIZE_WORD:
			if ((int16_t)data < 0)
				dest->l |= 0xffff0000;
			else
				dest->l &= 0xffff0000;
			break;
		}
	}
}

static uint32_t computeEA(uint8_t mode, uint8_t reg)
{
	/* the first two modes should NEVER call this function */
	
	switch (mode)
	{
	case 0b000: // data reg direct
		assert(0);
	case 0b001: // addr reg direct
		assert(0);
	case 0b010: // addr reg indirect
		assert(0);
	case 0b011: // addr reg indirect (post-increment)
		assert(0);
	case 0b100: // addr reg indirect (pre-increment)
		assert(0);
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

void instrNop(void)
{
	/* debug */
	dbgHeader();
	uartWritePSTR("nop\n");
	
	cpu.ureg.pc.l += 2;
	
	/* does not affect CCR */
}

void instrExg(void)
{
	/* debug */
	dbgHeader();
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
	dbgHeader();
	uartWritePSTR("move ccr,<ea>\n");
	
	cpu.ureg.pc.l += 2; // NOTE: may be larger for EA extensions
	
	/* get just the user portion of the SR */
	uint16_t ccr = cpu.ureg.sr.l & (SR_CARRY | SR_OVERFLOW | SR_ZERO |
		SR_NEGATIVE | SR_EXTEND);
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	if (mode <= 0b001) // reg direct
		writeReg(mode, reg, ccr, SIZE_WORD, false);
	else // eff addr
	{
		uint32_t dest = computeEA(mode, reg);
		memWrite(dest, SIZE_WORD, ccr);
	}
	
	/* does not affect CCR */
}

void instrMoveq(void)
{
	/* debug */
	dbgHeader();
	uartWritePSTR("moveq\n");
	
	cpu.ureg.pc.l += 2;
	
	uint8_t reg = (instr[0] & 0b1110) >> 1;
	int8_t data = instr[1];
	
	cpu.ureg.d[reg].l = instr[1];
	
	/* sign-extend to long */
	if (data < 0)
		cpu.ureg.d[reg].l |= 0xffffff00;
	
	/* update condition codes */
	cpu.ureg.sr.l &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if (data == 0)
		cpu.ureg.sr.l |= SR_ZERO;
	else if (data < 0)
		cpu.ureg.sr.l |= SR_NEGATIVE;
}

void instrClr(void)
{
	/* debug */
	dbgHeader();
	uartWritePSTR("clr\n");
	
	cpu.ureg.pc.l += 2; // NOTE: may be larger for EA extensions
	
	uint8_t size = instr[1] >> 6;
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	if (mode <= 0b001) // reg direct
		writeReg(mode, reg, 0, size, false);
	else
	{
		uint32_t dest = computeEA(mode, reg);
		memWrite(dest, size, 0);
	}
	
	cpu.ureg.sr.l &= ~(SR_CARRY | SR_OVERFLOW | SR_NEGATIVE);
	cpu.ureg.sr.l |= SR_ZERO;
}
