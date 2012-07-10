#include "m68k_instr.h"
#include "bitwise.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_ea.h"
#include "m68k_mem.h"
#include "uart.h"

extern const uint8_t *instr;

static void pushLong(uint32_t value)
{
	cpu.ureg.a[7].l -= 4;
	memWrite(cpu.ureg.a[7].l, SIZE_LONG, value);
}

static uint32_t popLong(void)
{
	uint32_t value = memRead(cpu.ureg.a[7].l, SIZE_LONG);
	cpu.ureg.a[7].l += 4;
	
	return value;
}

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
	instrLen += calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	/* get just the user portion of the SR */
	uint16_t ccr = cpu.ureg.sr.w[0] & (SR_CARRY | SR_OVERFLOW | SR_ZERO |
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
	cpu.ureg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if ((int8_t)data == 0)
		cpu.ureg.sr.b[0] |= SR_ZERO;
	else if ((int8_t)data < 0)
		cpu.ureg.sr.b[0] |= SR_NEGATIVE;
}

void instrClr(void)
{
	uartWritePSTR("clr <ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, size, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	accessEA(instr + 2, effAddr, mode, reg, 0, size, true);
	
	/* update condition codes */
	cpu.ureg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_NEGATIVE);
	cpu.ureg.sr.b[0] |= SR_ZERO;
}

void instrLea(void)
{
	uartWritePSTR("lea <ea>,%an\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, SIZE_LONG, &effAddr);
	
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
	instrLen += calcEA(instr + 2, mode, reg, SIZE_LONG, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	pushLong(effAddr);
	
	/* does not affect condition codes */
}

void instrMove(void)
{
	uartWritePSTR("move <ea>,<ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t srcMode = (instr[1] & 0b00111000) >> 3;
	uint8_t srcReg = instr[1] & 0b00000111;
	uint8_t size = ((instr[0] >> 4) & 0b11);
	
	/* convert the weird size into a normal size */
	if (size & 0b10)
		size ^= 0b01;
	--size;
	
	uint32_t srcAddr;
	const uint8_t *extWord1 = instr + instrLen;
	instrLen += calcEA(extWord1, srcMode, srcReg, size, &srcAddr);
	
	uint8_t dstMode = ((instr[0] & 0b1) << 2) | ((instr[1] >> 6) & 0b11);
	uint8_t dstReg = ((instr[0] >> 1) & 0b00000111);
	
	uint32_t dstAddr;
	const uint8_t *extWord2 = instr + instrLen;
	instrLen += calcEA(extWord2, dstMode, dstReg, size, &dstAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	/* get the data from the source */
	uint32_t data = accessEA(extWord1, srcAddr, srcMode, srcReg, 0, size,
		false);
	
	/* write the data to the destination */
	accessEA(extWord2, dstAddr, dstMode, dstReg, data, size, true);
	
	/* sign extend to make condition codes easier to calculate */
	if (size == SIZE_BYTE)
		data = signExtend8to32(data);
	else if (size == SIZE_WORD)
		data = signExtend16to32(data);
	else if (size != SIZE_LONG)
		assert(0);
	
	/* update condition codes */
	cpu.ureg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if ((int32_t)data == 0)
		cpu.ureg.sr.b[0] |= SR_ZERO;
	else if ((int32_t)data < 0)
		cpu.ureg.sr.b[0] |= SR_NEGATIVE;
}

void instrMoveFromSr(void)
{
	uartWritePSTR("move %sr,<ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	/* get the entire SR */
	uint16_t sr = cpu.ureg.sr.w[0];
	
	accessEA(instr + 2, effAddr, mode, reg, sr, SIZE_WORD, true);
	
	/* does not affect condition codes */
}

void instrMoveToCcr(void)
{
	uartWritePSTR("move <ea>,%ccr\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	/* get the new CCR value */
	uint16_t newCCR = accessEA(instr + 2, effAddr, mode, reg, 0, SIZE_WORD,
		false);
	
	/* store only the low-order byte */
	cpu.ureg.sr.b[0] = (uint8_t)newCCR;
	
	/* does not affect condition codes */
}

void instrNot(void)
{
	uartWritePSTR("not <ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, size, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	/* get the operand */
	uint32_t operand = accessEA(instr + 2, effAddr, mode, reg, 0, size, false);
	
	bool negative, zero;
	
	switch (size)
	{
	case SIZE_BYTE:
		operand ^= 0xff;
		negative = ((int8_t)operand < 0);
		zero = ((int8_t)operand == 0);
		break;
	case SIZE_WORD:
		operand ^= 0xffff;
		negative = ((int16_t)operand < 0);
		zero = ((int16_t)operand == 0);
		break;
	case SIZE_LONG:
		operand = ~operand;
		negative = ((int32_t)operand < 0);
		zero = ((int32_t)operand == 0);
		break;
	default:
		assert(0);
	}
	
	/* write the operand back */
	accessEA(instr + 2, effAddr, mode, reg, operand, size, true);
	
	/* update condition codes */
	cpu.ureg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if (zero)
		cpu.ureg.sr.b[0] |= SR_ZERO;
	else if (negative)
		cpu.ureg.sr.b[0] |= SR_NEGATIVE;
}

void instrJmp(void)
{
	uartWritePSTR("jmp <ea>\n");
	
	uint8_t instrLen = 2;
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	instrLen += calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	cpu.ureg.pc.l += instrLen;
	/* calculations can now take place */
	
	/* get the new program counter value */
	uint32_t newPC = accessEA(instr + 2, effAddr, mode, reg, 0, SIZE_LONG,
		false);
	uartWriteHex32(newPC, false); uartWriteChr('\n');
	uartWriteHex4(mode, false); uartWriteChr('\n');
	uartWriteHex4(reg, false); uartWriteChr('\n');
	
	/* store the new program counter */
	cpu.ureg.pc.l = newPC;
	
	/* does not affect condition codes */
}

void instrRts(void)
{
	uartWritePSTR("rts\n");
	
	cpu.ureg.pc.l += 2;
	/* calculations can now take place */
	
	cpu.ureg.pc.l = popLong();
	
	/* does not affect condition codes */
}
