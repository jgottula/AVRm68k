#include "m68k_instr.h"
#include "bitwise.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_ea.h"
#include "m68k_mem.h"
#include "uart.h"

extern uint8_t *instr;

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
