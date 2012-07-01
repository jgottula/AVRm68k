#include "m68k_instr.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_mem.h"
#include "uart.h"

extern uint8_t *instr;
extern uint16_t *instrWord;

#if 0
static void instrWriteOperand(uint32_t *dest, uint32_t data, uint8_t size)
{
	/* this works because AVR is little endian */
	uint16_t *destWord = (uint16_t *)dest;
	uint8_t *destByte = (uint8_t *)dest;
	
	switch (size)
	{
	case 8: // byte
		memWriteByte(*destByte, 
		*destByte = (uint8_t)data;
	}
}
#endif

static void instrWriteRegByte(Reg *reg, uint32_t data, uint8_t size)
{
	switch (size)
	{
	case 8:
		reg->b[0] = (uint8_t)data;
		break;
	case 16:
		reg->w[0] = (uint16_t)data;
		break;
	case 32:
		reg->w[0] = (uint16_t)data;
		break;
	default:
		assert(0);
	}
}

static void instrWriteEA(uint8_t mode, uint8_t reg, uint32_t data, uint8_t size)
{
	switch (mode)
	{
	case 0b000: // data reg direct
		instrWriteRegByte(&cpu.ureg.d[reg], data, size);
		break;
	case 0b001: // addr reg direct
		instrWriteRegByte(&cpu.ureg.a[reg], data, size);
		break;
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

void instrMoveCcr(void)
{
	/* debug */
	dbgHeader();
	uartWritePSTR("move ccr,<ea>\n");
	
	cpu.ureg.pc.l += 2;
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	/* get just the user portion of the SR */
	uint16_t ccr = cpu.ureg.sr.l & (SR_CARRY | SR_OVERFLOW | SR_ZERO |
		SR_NEGATIVE | SR_EXTEND);
	
	instrWriteEA(mode, reg, ccr, 16);
	
	/* does not affect CCR */
}
