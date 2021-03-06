/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

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
	uint8_t ccr = cpu.sreg.sr.b[0];
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

void instrEmu(void)
{
	uint16_t instrWord = decodeBigEndian16(instr);
	
	switch (instrWord)
	{
	case EMUINSTR_DUMPREG:
		uartWritePSTR("emu: dumpreg\n");
		m68kDumpReg();
		break;
	case EMUINSTR_DUMPMEM:
		uartWritePSTR("emu: dumpmem\n");
		uint32_t addr = decodeBigEndian32(instr + 2);
		uint16_t lines = decodeBigEndian16(instr + 6);
		memDump(addr, lines);
		cpu.ureg.pc.l += 6;
		break;
	default:
		assert(0);
	}
}

void instrAddq(void)
{
	uartWritePSTR("addq #<data>,<ea>\n");
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	uint8_t data = (instr[0] >> 1) & 0b00000111;
	
	/* special case for address registers: always use long size */
	if (mode == AMODE_AREGDIRECT)
		size = SIZE_LONG;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, size, &effAddr);
	
	/* get addend */
	uint32_t operand = accessEA(instr + 2, effAddr, mode, reg, 0, size, false);
	
	if (size == SIZE_BYTE)
		operand = signExtend8to32(operand);
	else if (size == SIZE_WORD)
		operand = signExtend16to32(operand);
	
	/* perform the add operation and immediately save the flags */
	operand += data;
	//uint8_t sr = getStatusReg();
	uartWritePSTR("this used to use getStatusReg! "
		"the emulator will now crash instead.\n");
	assert(0);
#if 0
	
	/* write back the sum */
	accessEA(instr + 2, effAddr, mode, reg, operand, size, true);
	
	/* update condition codes (except for address registers) */
	if (mode != AMODE_AREGDIRECT)
	{
		cpu.sreg.sr.b[0] = 0;
		
		if ((int8_t)operand == 0)
			cpu.sreg.sr.b[0] |= SR_ZERO;
		else if ((int8_t)operand < 0)
			cpu.sreg.sr.b[0] |= SR_NEGATIVE;
		
		if (sr & SREG_C)
			cpu.sreg.sr.b[0] |= SR_CARRY;
		if (sr & SREG_V)
			cpu.sreg.sr.b[0] |= SR_OVERFLOW;
	}
	
	cpu.ureg.pc.l += eaLen;
#endif
}

void instrAndEorOr(bool or, bool exclusive, bool dataRegDest)
{
	/* exclusive and not data dest should not happen */
	assert(!or || (!exclusive || !dataRegDest));
	
	if (or)
	{
		if (exclusive)
			uartWritePSTR("eor %dn,<ea>\n");
		else
		{
			if (dataRegDest)
				uartWritePSTR("or <ea>,%dn\n");
			else
				uartWritePSTR("or %dn,<ea>\n");
		}
	}
	else
	{
		if (dataRegDest)
			uartWritePSTR("and <ea>,%dn\n");
		else
			uartWritePSTR("and %dn,<ea>\n");
	}
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	
	uint8_t dataRegNum = ((instr[0] >> 1) & 0b00000111);
	Reg32 *dataReg = &cpu.ureg.d[dataRegNum];
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, size, &effAddr);
	
	/* get the second operand */
	uint32_t operand = accessEA(instr + 2, effAddr, mode, reg, 0, size, false);
	
	/* perform the operation */
	if (or)
	{
		if (exclusive)
			operand ^= dataReg->l;
		else
			operand |= dataReg->l;
	}
	else
		operand &= dataReg->l;
	
	bool negative, zero;
	
	switch (size)
	{
	case SIZE_BYTE:
		negative = ((int8_t)operand < 0);
		zero = ((int8_t)operand == 0);
		break;
	case SIZE_WORD:
		negative = ((int16_t)operand < 0);
		zero = ((int16_t)operand == 0);
		break;
	case SIZE_LONG:
		negative = ((int32_t)operand < 0);
		zero = ((int32_t)operand == 0);
		break;
	default:
		assert(0);
	}
	
	/* write the operand back */
	if (dataRegDest)
		accessEA(instr + 2, effAddr, AMODE_DREGDIRECT, dataRegNum, operand,
			size, true);
	else
		accessEA(instr + 2, effAddr, mode, reg, operand, size, true);
	
	/* update condition codes */
	cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if (zero)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else if (negative)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
	
	cpu.ureg.pc.l += eaLen;
}

void instrAndiEoriOri(bool or, bool exclusive)
{
	if (or)
	{
		if (exclusive)
			uartWritePSTR("eori #<data>,<ea>\n");
		else
			uartWritePSTR("ori #<data>,<ea>\n");
	}
	else
		uartWritePSTR("andi #<data>,<ea>\n");
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	
	/* get the first operand */
	uint32_t data;
	if (size == SIZE_BYTE)
		data = instr[3];
	else if (size == SIZE_WORD)
		data = decodeBigEndian16(instr + 2);
	else if (size == SIZE_LONG)
		data = decodeBigEndian32(instr + 2);
	else
		assert(0);
	
	uint8_t eaOffset = (size == SIZE_LONG ? 6 : 4);
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + eaOffset, mode, reg, size, &effAddr);
	
	/* get the second operand */
	uint32_t operand = accessEA(instr + eaOffset, effAddr, mode, reg, 0, size,
		false);
	
	/* perform the operation */
	if (or)
	{
		if (exclusive)
			operand ^= data;
		else
			operand |= data;
	}
	else
		operand &= data;
	
	/* write back the sum */
	accessEA(instr + eaOffset, effAddr, mode, reg, operand, size, true);
	
	bool negative, zero;
	
	switch (size)
	{
	case SIZE_BYTE:
		negative = ((int8_t)operand < 0);
		zero = ((int8_t)operand == 0);
		break;
	case SIZE_WORD:
		negative = ((int16_t)operand < 0);
		zero = ((int16_t)operand == 0);
		break;
	case SIZE_LONG:
		negative = ((int32_t)operand < 0);
		zero = ((int32_t)operand == 0);
		break;
	default:
		assert(0);
	}
	
	/* update condition codes */
	cpu.sreg.sr.b[0] = 0;
	if (zero)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else if (negative)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
	
	cpu.ureg.pc.l += eaLen + (eaOffset - 2);
}

void instrAslAsrMem(bool left)
{
	assert(0);
}

void instrAslAsrReg(bool left, bool immediate)
{
	if (left)
		uartWritePSTR("asl ");
	else
		uartWritePSTR("asr ");
	if (immediate)
		uartWritePSTR("#<data>,%dy\n");
	else
		uartWritePSTR("%dx,%dy\n");
	
	uint8_t count = (instr[0] & 0b00001110) >> 1;
	uint8_t size = instr[1] >> 6;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint8_t shifts;
	
	/* the number of shifts to perform may be immediate or from a register */
	if (immediate)
		shifts = count;
	else
		shifts = cpu.ureg.d[count].b[0] % 64;
	
	uint32_t operand, result, msbMask;
	
	/* load the data register to be shifted; also note which bit is the msb */
	switch (size)
	{
	case SIZE_BYTE:
		operand = signExtend8to32(cpu.ureg.d[reg].b[0]);
		msbMask = 0x80;
		break;
	case SIZE_WORD:
		operand = signExtend16to32(cpu.ureg.d[reg].w[0]);
		msbMask = 0x8000;
		break;
	case SIZE_LONG:
		operand = cpu.ureg.d[reg].l;
		msbMask = 0x80000000;
		break;
	default:
		assert(0);
	}
	
	if (shifts == 0)
	{
		/* update condition codes for no-shift operation */
		cpu.sreg.sr.b[0] &= ~(SR_OVERFLOW | SR_CARRY);
		
		result = operand;
	}
	else
	{
		uint8_t flags;
		
		if (left)
			result = shiftLeftArithLong(operand, shifts, &flags);
		else
			result = shiftRightArithLong(operand, shifts, &flags);
		
		/* store the result back to the data register */
		switch (size)
		{
		case SIZE_BYTE:
			cpu.ureg.d[reg].b[0] = result;
			break;
		case SIZE_WORD:
			cpu.ureg.d[reg].w[0] = result;
			break;
		case SIZE_LONG:
			cpu.ureg.d[reg].l = result;
			break;
		default:
			assert(0);
		}
		
		/* update condition codes for operation with shifts > 0 */
		cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_EXTEND);
		cpu.sreg.sr.b[0] |= flags;
	}
	
	/* update condition codes for all operations */
	if (result == 0)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else
		cpu.sreg.sr.b[0] &= ~SR_ZERO;
	if ((result & msbMask) == msbMask)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
	else
		cpu.sreg.sr.b[0] &= ~SR_NEGATIVE;
}

void instrBcc(void)
{
	uartWritePSTR("bcc <ea>\n");
	
	bool cond = condTest(instr[0] & 0b00001111);
	
	uint8_t dispByte = instr[1];
	
	if (cond)
	{
		if (dispByte == 0x00) // word
			cpu.ureg.pc.l += signExtend16to32(decodeBigEndian16(instr + 2));
		else if (dispByte == 0xff) // long
			cpu.ureg.pc.l += decodeBigEndian32(instr + 2);
		else // byte
			cpu.ureg.pc.l += signExtend8to32(dispByte);
	}
	else
	{
		if (dispByte == 0x00) // word
			cpu.ureg.pc.l += 2;
		else if (dispByte == 0xff) // long
			cpu.ureg.pc.l += 4;
		else // byte
			cpu.ureg.pc.l += 2;
	}
	
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

void instrBsetBtst(bool set, bool immediate)
{
	if (set)
		uartWritePSTR("bset");
	else
		uartWritePSTR("btst");
	
	if (immediate)
		uartWritePSTR(" #<data>,<ea>\n");
	else
		uartWritePSTR(" %dn,<ea>\n");
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	
	uint8_t size = (mode == AMODE_DREGDIRECT ? SIZE_LONG : SIZE_BYTE);
	
	uint8_t offset = (immediate ? 2 : 0);
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2 + offset, mode, reg, size, &effAddr);
	
	uint8_t regNum = (instr[0] & 0b00001110) >> 1;
	uint8_t bitNum;
	
	if (immediate)
		bitNum = instr[3];
	else
		bitNum = cpu.ureg.d[regNum].b[0];
	
	uint32_t test = accessEA(instr + 2 + offset, effAddr, mode, reg, 0, size,
		false);
	
	bool zero;
	
	/* operation is 32-bit if eff addr is a data register; 8-bit otherwise */
	if (mode == AMODE_DREGDIRECT)
	{
		assert(bitNum < 32);
		
		zero = ((test & (1 << bitNum)) == 0);
	}
	else
	{
		assert(bitNum < 8);
		
		zero = ((test & (1 << bitNum)) == 0);
	}
	
	/* for bset, set the bit we tested */
	if (set)
		accessEA(instr + 2 + offset, effAddr, mode, reg, test | (1 << bitNum),
			size, true);
	
	/* update condition codes */
	if (zero)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else
		cpu.sreg.sr.b[0] &= ~SR_ZERO;
	
	cpu.ureg.pc.l += eaLen + offset;
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
	cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_NEGATIVE);
	cpu.sreg.sr.b[0] |= SR_ZERO;
	
	cpu.ureg.pc.l += eaLen;
}

void instrDbcc(void)
{
	uartWritePSTR("dbcc %dn,<ea>\n");
	
	bool cond = condTest(instr[0] & 0b00001111);
	
	uint8_t reg = instr[1] & 0b00000111;
	
	uint16_t displacement = decodeBigEndian16(instr + 2);
	
	/* only decrement/branch if the condition is false */
	if (!cond)
	{
		/* decrement; then branch if not equal to -1 */
		if ((int16_t)--cpu.ureg.d[reg].w[0] != -1)
		{
			cpu.ureg.pc.l += signExtend16to32(displacement);
			
			/* return early to avoid pc advancement */
			return;
		}
	}
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += 2;
}

void instrExg(void)
{
	uartWritePSTR("exg %dn|%an,%dn|%an\n");
	
	uint8_t regIdx1 = (instr[0] & 0b00001110) >> 1;
	uint8_t regIdx2 = (instr[1] & 0b00000111);
	
	Reg32 *reg1, *reg2;
	
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
	
	Reg32 temp = *reg1;
	*reg1 = *reg2;
	*reg2 = temp;
	
	/* does not affect condition codes */
}

void instrExt(void)
{
	uartWritePSTR("ext %dn\n");
	
	uint8_t opMode = (((instr[0] & 0b00000001) << 2) | (instr[1] >> 6));
	uint8_t reg = instr[1] & 0b00000111;
	
	bool negative, zero;
	
	switch (opMode)
	{
	case 0b010: // byte to word
		cpu.ureg.d[reg].w[0] = signExtend8to16(cpu.ureg.d[reg].b[0]);
		negative = ((int16_t)cpu.ureg.d[reg].w[0] < 0);
		zero = (cpu.ureg.d[reg].w[0] == 0);
		break;
	case 0b011: // word to long
		cpu.ureg.d[reg].l = signExtend16to32(cpu.ureg.d[reg].w[0]);
		negative = ((int32_t)cpu.ureg.d[reg].l < 0);
		zero = (cpu.ureg.d[reg].l == 0);
		break;
	case 0b111: // byte to long
		cpu.ureg.d[reg].l = signExtend8to32(cpu.ureg.d[reg].b[0]);
		negative = ((int32_t)cpu.ureg.d[reg].l < 0);
		zero = (cpu.ureg.d[reg].l == 0);
		break;
	default:
		assert(0);
	}
	
	/* update condition codes */
	cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if (zero)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else if (negative)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
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

void instrLink(uint8_t size)
{
	uartWritePSTR("link %an,#<displacement>\n");
	
	uint8_t reg = instr[1] & 0b00000111;
	
	uint32_t displacement;
	uint8_t dispLen;
	
	switch (size)
	{
	case SIZE_WORD:
		displacement = signExtend16to32(decodeBigEndian16(instr + 2));
		dispLen = 2;
		break;
	case SIZE_LONG:
		displacement = decodeBigEndian32(instr + 2);
		dispLen = 4;
		break;
	default:
		assert(0);
	}
	
	/* push the specified address register onto the stack */
	pushLong(cpu.ureg.a[reg].l);
	
	/* load the updated stack pointer into the address register */
	cpu.ureg.a[reg].l = cpu.ureg.a[7].l;
	
	/* add the displacement value to the stack pointer */
	cpu.ureg.a[7].l += displacement;
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += dispLen;
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
	cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if ((int32_t)data == 0)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else if ((int32_t)data < 0)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
	
	cpu.ureg.pc.l += eaLen;
}

void instrMoveFromCcr(void)
{
	uartWritePSTR("move %ccr,<ea>\n");
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	/* get just the user portion of the SR */
	uint16_t ccr = cpu.sreg.sr.b[0] & (SR_CARRY | SR_OVERFLOW | SR_ZERO |
		SR_NEGATIVE | SR_EXTEND);
	
	accessEA(instr + 2, effAddr, mode, reg, ccr, SIZE_WORD, true);
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}

void instrMoveFromSr(void)
{
	uartWritePSTR("move %sr,<ea>\n");
	
	/* privileged instruction */
	assert(cpu.sreg.sr.w & SR_USERSTATE);
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	/* get the entire SR */
	uint16_t sr = cpu.sreg.sr.w;
	
	accessEA(instr + 2, effAddr, mode, reg, sr, SIZE_WORD, true);
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}

static void instrMovemInternal(bool regToMem, uint8_t mode, uint8_t reg,
	uint8_t size, uint32_t effAddr, uint8_t *offset, uint32_t postDecAddr,
	bool addrReg, uint8_t regNum)
{
	assert(size == SIZE_WORD || size == SIZE_LONG);
	
	if (regToMem)
	{
		uint32_t value;
			
		/* special case for predec mode: write out the initial value of the 
		 * register, minus the size of the operation */
		if (mode == AMODE_AREGPREDEC && addrReg && regNum == reg)
			value = postDecAddr;
		else
		{
			if (addrReg)
				value = cpu.ureg.a[regNum].l;
			else
				value = cpu.ureg.d[regNum].l;
		}
		
		accessEA(instr + 4, effAddr + *offset, mode, reg, value, size, true);
	}
	else
	{
		uint32_t value;
		
		value = accessEA(instr + 4, effAddr + *offset, mode, reg, 0, size,
			false);
		
		/* special case for postinc mode: don't overwrite that register */
		if (mode != AMODE_AREGPOSTINC || !addrReg || regNum != reg)
		{
			if (size == SIZE_LONG)
			{
				if (addrReg)
					cpu.ureg.a[regNum].l = value;
				else
					cpu.ureg.d[regNum].l = value;
			}
			else
			{
				if (addrReg)
					cpu.ureg.a[regNum].l = signExtend16to32(value);
				else
					cpu.ureg.d[regNum].l = signExtend16to32(value);
			}
		}
	}
	
	/* not sure why this needs be run for postinc, but this seems to work */
	if (mode != AMODE_AREGPREDEC)
	{
		if (size == SIZE_LONG)
			*offset += 4;
		else if (size == SIZE_WORD)
			*offset += 2;
	}
}

void instrMovem(bool regToMem)
{
	if (regToMem)
		uartWritePSTR("movem <list>,<ea>\n");
	else
		uartWritePSTR("movem <ea>,<list>\n");
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	uint8_t size = ((instr[1] & 0b01000000) ? SIZE_LONG : SIZE_WORD);
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 4, mode, reg, size, &effAddr);
	
	uint16_t mask = decodeBigEndian16(instr + 2);
	
	uint16_t bit;
	uint8_t offset = 0;
	
	uint32_t postDecAddr = effAddr - _BV(size);
	
	/* for predecrement mode, the register order is switched */
	if (mode != AMODE_AREGPREDEC)
	{
		bit = _BV(0);
		
		for (int8_t i = 0; i < 8; ++i)
		{
			if (mask & bit)
				instrMovemInternal(regToMem, mode, reg, size, effAddr, &offset,
					postDecAddr, false, i);
			
			bit <<= 1;
		}
		
		for (int8_t i = 0; i < 8; ++i)
		{
			if (mask & bit)
				instrMovemInternal(regToMem, mode, reg, size, effAddr, &offset,
					postDecAddr, true, i);
			
			bit <<= 1;
		}
	}
	else
	{
		bit = _BV(0);
		
		for (int8_t i = 7; i >= 0; --i)
		{
			if (mask & bit)
				instrMovemInternal(regToMem, mode, reg, size, effAddr, &offset,
					postDecAddr, true, i);
			
			bit <<= 1;
		}
		
		for (int8_t i = 7; i >= 0; --i)
		{
			if (mask & bit)
				instrMovemInternal(regToMem, mode, reg, size, effAddr, &offset,
					postDecAddr, false, i);
			
			bit <<= 1;
		}
	}
	
	cpu.ureg.pc.l += eaLen + 2;
}

void instrMoveq(void)
{
	uartWritePSTR("moveq #<data>,%dn\n");
	
	uint8_t reg = (instr[0] & 0b1110) >> 1;
	uint8_t data = instr[1];
	
	cpu.ureg.d[reg].l = signExtend8to32(instr[1]);
	
	/* update condition codes */
	cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if ((int8_t)data == 0)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else if ((int8_t)data < 0)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
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
	cpu.sreg.sr.b[0] = (uint8_t)newCCR;
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}

void instrMoveToSr(void)
{
	uartWritePSTR("move <ea>,%sr\n");
	
	/* privileged instruction */
	assert(cpu.sreg.sr.w & SR_USERSTATE);
	
	uint8_t mode = (instr[1] >> 3) & 0b111;
	uint8_t reg = instr[1] & 0b111;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, SIZE_WORD, &effAddr);
	
	/* get the new SR value */
	uint16_t newSR = accessEA(instr + 2, effAddr, mode, reg, 0, SIZE_WORD,
		false);
	
	/* store the whole word */
	cpu.sreg.sr.w = newSR;
	
	/* does not affect condition codes */
	
	cpu.ureg.pc.l += eaLen;
}

void instrMoveUsp(bool toUSP)
{
	if (toUSP)
		uartWritePSTR("move %an,%usp\n");
	else
		uartWritePSTR("move %usp,%an\n");
	
	/* privileged instruction */
	assert(cpu.sreg.sr.w & SR_USERSTATE);
	
	uint8_t reg = instr[1] & 0b111;
	
	/* move the user stack pointer to/from the address reg */
	if (toUSP)
		cpu.ureg.a[7] = cpu.ureg.a[reg];
	else
		cpu.ureg.a[reg] = cpu.ureg.a[7];
	
	/* does not affect condition codes */
}

void instrNeg(bool extend)
{
	if (extend)
		uartWritePSTR("negx <ea>\n");
	else
		uartWritePSTR("neg <ea>\n");
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, size, &effAddr);
	
	/* get the operand */
	uint32_t operand = accessEA(instr + 2, effAddr, mode, reg, 0, size, false);
	
	/* the result is sign extended here to facilitate flag checking later; the
	 * byte and word modes will work as intended by truncating the value */
	switch (size)
	{
	case SIZE_BYTE:
		operand = signExtend8to32(operand);
		break;
	case SIZE_WORD:
		operand = signExtend16to32(operand);
		break;
	case SIZE_LONG:
		break;
	default:
		assert(0);
	}
	
	if (extend && (cpu.sreg.sr.b[0] & SR_EXTEND) != 0)
		++operand;
	
	operand = -operand;
	//uint8_t sr = getStatusReg();
	uartWritePSTR("this used to use getStatusReg! "
		"the emulator will now crash instead.\n");
	assert(0);
#if 0
	
	/* write the operand back */
	accessEA(instr + 2, effAddr, mode, reg, operand, size, true);
	
	/* update condition codes */
	if (!extend)
	{
		/* carry and extend are set by default */
		cpu.sreg.sr.b[0] = SR_CARRY | SR_EXTEND;
		
		if (operand == 0)
		{
			cpu.sreg.sr.b[0] |= SR_ZERO;
			cpu.sreg.sr.b[0] &= ~SR_CARRY & ~SR_EXTEND;
		}
		else if ((int32_t)operand < 0)
			cpu.sreg.sr.b[0] |= SR_NEGATIVE;
		
		if ((sr & SREG_V) != 0)
			cpu.sreg.sr.b[0] |= SR_OVERFLOW;
	}
	else
	{
		/* don't touch zero except to clear it if applicable */
		cpu.sreg.sr.b[0] &= ~SR_CARRY & ~SR_OVERFLOW & ~SR_NEGATIVE &
			~SR_EXTEND;
		
		if (operand != 0)
		{
			cpu.sreg.sr.b[0] &= ~SR_ZERO;
			
			if ((int32_t)operand < 0)
				cpu.sreg.sr.b[0] |= SR_NEGATIVE;
		}
		
		/* set extend the same as carry */
		if ((sr & SREG_C) != 0)
			cpu.sreg.sr.b[0] |= SR_CARRY | SR_EXTEND;
		
		if ((sr & SREG_V) != 0)
			cpu.sreg.sr.b[0] |= SR_OVERFLOW;
	}
	
	cpu.ureg.pc.l += eaLen;
#endif
}

void instrNop(void)
{
	uartWritePSTR("nop\n");
	
	/* does not affect condition codes */
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
	cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if (zero)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else if (negative)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
	
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

void instrRts(void)
{
	uartWritePSTR("rts\n");
	
	/* program counter increment not necessary (pc overwritten; no ea calc) */
	
	cpu.ureg.pc.l = popLong();
	
	/* does not affect condition codes */
}

void instrScc(void)
{
	uartWritePSTR("scc <ea>\n");
	
	bool cond = condTest(instr[0] & 0b00001111);
	
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

void instrSwap(void)
{
	uartWritePSTR("swap %dn\n");
	
	uint8_t reg = instr[1] & 0b111;
	
	uint16_t lsw, msw;
	
	lsw = cpu.ureg.d[reg].w[0];
	msw = cpu.ureg.d[reg].w[1];
	
	cpu.ureg.d[reg].w[0] = msw;
	cpu.ureg.d[reg].w[1] = lsw;
	
	/* update condition codes */
	cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if (lsw == 0 && msw == 0)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else if (lsw & 0b1000000000000000)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
}

void instrTst(void)
{
	uartWritePSTR("tst <ea>\n");
	
	uint8_t mode = (instr[1] & 0b00111000) >> 3;
	uint8_t reg = instr[1] & 0b00000111;
	uint8_t size = instr[1] >> 6;
	
	uint32_t effAddr;
	uint8_t eaLen = calcEA(instr + 2, mode, reg, size, &effAddr);
	
	uint32_t test = accessEA(instr + 2, effAddr, mode, reg, 0, size, false);
	
	if (size == SIZE_BYTE)
		test = signExtend8to32(test);
	else if (size == SIZE_WORD)
		test = signExtend16to32(test);
	
	/* update condition codes */
	cpu.sreg.sr.b[0] &= ~(SR_CARRY | SR_OVERFLOW | SR_ZERO | SR_NEGATIVE);
	if ((int32_t)test == 0)
		cpu.sreg.sr.b[0] |= SR_ZERO;
	else if ((int32_t)test < 0)
		cpu.sreg.sr.b[0] |= SR_NEGATIVE;
	
	cpu.ureg.pc.l += eaLen;
}

void instrUnlk(void)
{
	uartWritePSTR("unlk %an\n");
	
	uint8_t reg = instr[1] & 0b00000111;
	
	/* load the stack pointer from the specified address register */
	cpu.ureg.a[7].l = cpu.ureg.a[reg].l;
	
	/* pop the value on the top of the stack into the address register */
	cpu.ureg.a[reg].l = popLong();
	
	/* does not affect condition codes */
}
