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

static bool condTest(uint8_t nibble)
{
	uint8_t ccr = cpu.ureg.sr.b[1];
	bool carry = ((ccr & SR_CARRY) != 0);
	bool overflow = ((ccr & SR_OVERFLOW) != 0);
	bool zero = ((ccr & SR_ZERO) != 0);
	bool negative = ((ccr & SR_NEGATIVE) != 0);
	
	/* order the cases AND the expressions to speed up common cases */
	
	switch (nibble)
	{
	case COND_T: // true
		return true;
	case COND_F: // false
		return false;
	case COND_HI: // high
		return (!carry && !zero);
	case COND_LS: // low or same
		return (carry || zero);
	case COND_CC: // carry clear
		return !carry;
	case COND_CS: // carry set
		return carry;
	case COND_NE: // not equal
		return !zero;
	case COND_EQ: // equal
		return zero;
	case COND_VC: // overflow clear
		return !overflow;
	case COND_VS: // overflow set
		return overflow;
	case COND_PL: // plus
		return !negative;
	case COND_MI: // minus
		return negative;
	case COND_GE: // greater or equal
		return ((negative && overflow) || (!negative && !overflow));
	case COND_LT: // less than
		return ((negative && !overflow) || (!negative && overflow));
	case COND_GT: // greater than
		return ((negative && overflow && !zero) ||
			(!negative && !overflow && !zero));
	case COND_LE: // less or equal
		return ((negative && !overflow) || (!negative && overflow) || zero);
	default:
		assert(0);
	}
}

bool instrEmu(void)
{
	uint16_t instrWord = decodeBigEndian16(instr);
	
	switch (instrWord)
	{
	case EMUINSTR_DUMPREG:
		uartWritePSTR("emu: dumpreg\n");
		m68kDumpReg();
		return true;
	case EMUINSTR_DUMPMEM:
		uartWritePSTR("emu: dumpmem\n");
		uint32_t addr = decodeBigEndian32(instr + 2);
		uint16_t lines = decodeBigEndian16(instr + 6);
		memDump(addr, lines);
		cpu.ureg.pc.l += 6;
		return true;
	default:
		return false;
	}
}

void instrNop(void)
{
	uartWritePSTR("nop\n");
	
	/* does not affect condition codes */
}

void instrExg(void)
{
	uartWritePSTR("exg %dn|%an,%dn|%an\n");
	
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
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	/* get just the user portion of the SR */
	uint16_t ccr = cpu.ureg.sr.w[0] & (SR_CARRY | SR_OVERFLOW | SR_ZERO |
		SR_NEGATIVE | SR_EXTEND);
	
	accessEA(instr + 2, effAddr, mode, reg, ccr, SIZE_WORD, true);
	
	cpu.ureg.pc.l += eaLen;
	
	/* does not affect condition codes */
}

void instrMoveq(void)
{
	uartWritePSTR("moveq #<data>,%dn\n");
	
	uint8_t reg = (instr[0] & 0b1110) >> 1;
	uint8_t data = instr[1];
	
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
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, size, &effAddr);
	
	accessEA(instr + 2, effAddr, mode, reg, 0, size, true);
	
	/* update condition codes */
	cpu.ureg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_NEGATIVE);
	cpu.ureg.sr.b[0] |= SR_ZERO;
	
	cpu.ureg.pc.l += eaLen;
}

void instrLea(void)
{
	uartWritePSTR("lea <ea>,%an\n");
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_LONG, &effAddr);
	
	uint8_t actualReg = (instr[0] >> 1) & 0b111;
	cpu.ureg.a[actualReg].l = effAddr;
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}

void instrPea(void)
{
	uartWritePSTR("pea <ea>\n");
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_LONG, &effAddr);
	
	pushLong(effAddr);
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}

void instrMove(void)
{
	uartWritePSTR("move <ea>,<ea>\n");
	
	uint8_t srcMode = (instr[1] & 0b00111000) >> 3;
	uint8_t srcReg = instr[1] & 0b00000111;
	uint8_t size = ((instr[0] >> 4) & 0b11);
	
	/* convert the weird size into a normal size */
	if (size & 0b10)
		size ^= 0b01;
	--size;
	
	uint32_t srcAddr;
	const uint8_t *extWord1 = instr + 2;
	uint8_t eaLen = calcEA(extWord1, srcMode, srcReg, size, &srcAddr);
	
	uint8_t dstMode = ((instr[0] & 0b1) << 2) | ((instr[1] >> 6) & 0b11);
	uint8_t dstReg = ((instr[0] >> 1) & 0b00000111);
	
	uint32_t dstAddr;
	const uint8_t *extWord2 = instr + eaLen;
	eaLen += calcEA(extWord2, dstMode, dstReg, size, &dstAddr);
	
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
	
	cpu.ureg.pc.l += eaLen;
}

void instrMoveFromSr(void)
{
	uartWritePSTR("move %sr,<ea>\n");
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	/* get the entire SR */
	uint16_t sr = cpu.ureg.sr.w[0];
	
	accessEA(instr + 2, effAddr, mode, reg, sr, SIZE_WORD, true);
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}

void instrMoveToCcr(void)
{
	uartWritePSTR("move <ea>,%ccr\n");
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	/* get the new CCR value */
	uint16_t newCCR = accessEA(instr + 2, effAddr, mode, reg, 0, SIZE_WORD,
		false);
	
	/* store only the low-order byte */
	cpu.ureg.sr.b[0] = (uint8_t)newCCR;
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}

void instrNot(void)
{
	uartWritePSTR("not <ea>\n");
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, size, &effAddr);
	
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
	
	cpu.ureg.pc.l += eaLen;
}

void instrJmp(void)
{
	uartWritePSTR("jmp <ea>\n");
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	/* store the new program counter */
	cpu.ureg.pc.l = effAddr;
	
	/* does not affect condition codes */
}

void instrRts(void)
{
	uartWritePSTR("rts\n");
	
	/* program counter increment not necessary (pc overwritten; no ea calc) */
	
	cpu.ureg.pc.l = popLong();
	
	/* does not affect condition codes */
}

void instrJsr(void)
{
	uartWritePSTR("jsr <ea>\n");
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	/* push the current program counter */
	pushLong(cpu.ureg.pc.l + eaLen);
	
	/* store the new program counter */
	cpu.ureg.pc.l = effAddr;
	
	/* does not affect condition codes */
}

void instrBra(void)
{
	uartWritePSTR("bra <ea>\n");
	
	uint8_t dispByte = instr[1];
	
	if (dispByte == 0x00) // word
		cpu.ureg.pc.l += signExtend16to32(decodeBigEndian16(instr + 2));
	else if (dispByte == 0xff) // long
		cpu.ureg.pc.l += decodeBigEndian32(instr + 2);
	else // byte
		cpu.ureg.pc.l += signExtend8to32(dispByte);
	
	/* does not affect condition codes */
}

void instrBsr(void)
{
	uartWritePSTR("bsr <ea>\n");
	
	uint8_t dispByte = instr[1];
	uint32_t add;
	uint8_t size = 1;
	
	if (dispByte == 0x00) // word
	{
		add = signExtend16to32(decodeBigEndian16(instr + 2));
		size = 2;
	}
	else if (dispByte == 0xff) // long
	{
		add = decodeBigEndian32(instr + 2);
		size = 4;
	}
	else // byte
		add = signExtend8to32(dispByte);
	
	/* push the current program counter */
	pushLong(cpu.ureg.pc.l + size);
	
	/* store the new program counter */
	cpu.ureg.pc.l += add;
	
	/* does not affect condition codes */
}

void instrScc(void)
{
	uartWritePSTR("scc <ea>\n");
	
	bool cond = condTest(instr[0] & 0b00001111);
	uartWriteBool(cond);
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_BYTE, &effAddr);
	
	/* write all ones if true, all zeroes if false */
	if (cond)
		accessEA(instr + 2, effAddr, mode, reg, 0xff, SIZE_BYTE, true);
	else
		accessEA(instr + 2, effAddr, mode, reg, 0x00, SIZE_BYTE, true);
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}


