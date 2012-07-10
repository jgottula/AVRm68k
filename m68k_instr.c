#include "m68k_instr.h"
#include "bitwise.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_ea.h"
#include "m68k_mem.h"
#include "uart.h"

extern const uint8_t *instr;

bool instrEmu(void)
{
	uint16_t instrWord = decodeBigEndian16(instr);
	
	switch (instrWord)
	{
	case EMUINSTR_DUMPREG:
		uartWritePSTR("emu: dumpreg\n");
		cpu.ureg.pc.l += 2;
		m68kDumpReg();
		return true;
	case EMUINSTR_DUMPMEM:
		uartWritePSTR("emu: dumpmem\n");
		cpu.ureg.pc.l += 8;
		uint32_t addr = decodeBigEndian32(instr + 2);
		uint16_t lines = decodeBigEndian16(instr + 6);
		memDump(addr, lines);
		return true;
	default:
		return false;
	}
}

void instrNop(void)
{
	uartWritePSTR("nop\n");
	
	cpu.ureg.pc.l += 2;
	
	/* does not affect condition codes */
}

void instrExg(void)
{
	uartWritePSTR("exg %dn|%an,%dn|%an\n");
	
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
	
	/* does not affect condition codes */
}

void instrMoveFromCcr(void)
{
	uartWritePSTR("move %ccr,<ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	/* get just the user portion of the SR */
	uint16_t ccr = cpu.ureg.sr.l & (SR_CARRY | SR_OVERFLOW | SR_ZERO |
		SR_NEGATIVE | SR_EXTEND);
	
	accessEA(instr + 2, effAddr, mode, reg, ccr, SIZE_WORD, true);
	
	/* does not affect condition codes */
}

void instrMoveq(void)
{
	uartWritePSTR("moveq #<data>,%dn\n");
	
	uint8_t reg = (instr[0] & 0b1110) >> 1;
	uint8_t data = instr[1];
	
	cpu.ureg.pc.l += 2;
	/* calculations can now take place */
	
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
	uartWritePSTR("clr <ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	uint8_t size = instr[1] >> 6;
	
	accessEA(instr + 2, effAddr, mode, reg, 0, size, true);
	
	/* update condition codes */
	cpu.ureg.sr.l &= ~(SR_CARRY | SR_OVERFLOW | SR_NEGATIVE);
	cpu.ureg.sr.l |= SR_ZERO;
}

void instrLea(void)
{
	uartWritePSTR("lea <ea>,%an\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	uint8_t actualReg = (instr[0] >> 1) & 0b111;
	cpu.ureg.a[actualReg].l = effAddr;
	
	/* does not affect condition codes */
}

void instrPea(void)
{
	uartWritePSTR("pea <ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	cpu.ureg.a[7].l -= 4;
	memWrite(cpu.ureg.a[7].l, SIZE_LONG, effAddr);
	
	/* does not affect condition codes */
}

void instrMovea(void)
{
	uartWritePSTR("movea <ea>,%an\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	uint8_t size = ((instr[0] >> 4) & 0b11);
	
	/* convert the weird size into a normal size */
	if (size & 0b10)
		size ^= 0b01;
	--size;
	
	uint32_t newAddr = accessEA(instr + 2, effAddr, mode, reg, 0, size, false);
	if (size == SIZE_WORD)
		newAddr = signExtend16to32(newAddr);
	
	uint8_t actualReg = (instr[0] >> 1) & 0b111;
	cpu.ureg.a[actualReg].l = newAddr;
	
	/* does not affect condition codes */
}

void instrMoveFromSr(void)
{
	uartWritePSTR("move %sr,<ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	/* get the entire SR */
	uint16_t sr = cpu.ureg.sr.w[0];
	
	accessEA(instr + 2, effAddr, mode, reg, sr, SIZE_WORD, true);
	
	/* does not affect condition codes */
}
