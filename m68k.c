#include "m68k.h"
#include <avr/eeprom.h>
#include "debug.h"
#include "m68k_instr.h"
#include "uart.h"

static uint8_t fetchBuffer[22]; // must have even size
const uint8_t *instr;

static void m68kFetch(void)
{
	eeprom_read_block(fetchBuffer, (const void *)(intptr_t)cpu.ureg.pc.l,
		sizeof(fetchBuffer));
	
	/* debug */
	uartWritePSTR("\n[Fetch 0x");
	uartWriteHex32(cpu.ureg.pc.l, false);
	uartWriteChr(']');
	for (uint8_t i = 0; i < sizeof(fetchBuffer) / 2; ++i)
	{
		uartWriteChr(' ');
		uartWriteHex8(fetchBuffer[i * 2], false);
		uartWriteHex8(fetchBuffer[(i * 2) + 1], false);
	}
	uartWriteChr('\n');
}

static void m68kDecode(void)
{
	instr = fetchBuffer;
	
	uartWritePSTR("[Instr] ");
	
	assert(cpu.ureg.pc.l % 2 == 0);
	
	/* advance the program counter past the instruction word */
	cpu.ureg.pc.l += 2;
	
	/* ensure that more likely instructions are first in if/else blocks */
	
	/* separate operations by front nibble */
	switch (instr[0] >> 4)
	{
	case 0b0000: // bit manipulation, MOVEP, immediate
	{
		switch (instr[0] & 0b1111)
		{
		case 0b0000:
			if ((instr[1] & 0b11000000) != 0b11000000) // size
			{
				if ((instr[1] & 0b00111000) != 0b00001000) // mode
					instrEoriOriAndi(true, false); // ori
				else
					assert(0);
			}
			else
				assert(0); /* instruction with size bits goes here */
			break;
		case 0b1010:
			if ((instr[1] & 0b11000000) != 0b11000000) // size
			{
				if ((instr[1] & 0b00111000) != 0b00001000) // mode
					instrEoriOriAndi(true, true); // eori
				else
					assert(0);
			}
			else
				assert(0); /* instruction with size bits goes here */
			break;
		case 0b0010:
			if ((instr[1] & 0b11000000) != 0b11000000) // size
			{
				if ((instr[1] & 0b00111000) != 0b00001000) // mode
					instrEoriOriAndi(false, false); // andi
				else
					assert(0);
			}
			else
				assert(0); /* instruction with size bits goes here */
			break;
		default:
			assert(0);
		}
		break;
	}
	case 0b0001: // move byte
	case 0b0010: // move long
	case 0b0011: // move word
	{
		instrMove();
		break;
	}
	case 0b0100: // miscellaneous
	{
		/* this is a badly written special case for lea, since it puts a
		 * register field right in the second nibble */
		if ((instr[0] & 0b00000001) == 0b00000001 &&
			(instr[1] & 0b11000000) == 0b11000000)
		{
			switch ((instr[1] & 0b00111000) >> 3)
			{
				case 0b010:
				case 0b101:
				case 0b110:
				case 0b111:
					instrLea();
					break;
				case 0b000:
					instrExt(); // extb
					break;
				default:
					assert(0);
			}
		}
		else
		{
			switch (instr[0] & 0b1111)
			{
			case 0b0000:
			{
				if ((instr[1] & 0b11000000) == 0b11000000) // size
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrMoveFromSr();
					else
						assert(0);
				}
				else
					assert(0); /* instruction with size bits goes here */
				break;
			}
			case 0b0010:
			{
				if ((instr[1] & 0b11000000) == 0b11000000) // size
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrMoveFromCcr();
					else
						assert(0);
				}
				else
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrClr();
					else
						assert(0);
				}
				break;
			}
			case 0b0100:
			{
				if ((instr[1] & 0b11000000) == 0b11000000) // size
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrMoveToCcr();
					else
						assert(0);
				}
				else
					assert(0); /* instruction with size bits goes here */
				break;
			}
			case 0b0110:
			{
				if ((instr[1] & 0b11000000) == 0b11000000) // size
				{
					assert(0); /* instruction without size bits goes here */
				}
				else
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrNot();
					else
						assert(0);
				}
				break;
			}
			case 0b1000:
			{
				if ((instr[1] & 0b11000000) == 0b01000000) // size
				{
					switch ((instr[1] & 0b00111000) >> 3) // mode
					{
					case 0b010:
					case 0b101:
					case 0b110:
					case 0b111:
						instrPea();
						break;
					case 0b000:
						instrSwap();
						break;
					default:
						assert(0);
					}
				}
				else if ((instr[1] & 0b11000000) == 0b10000000 ||
					(instr[1] & 0b11000000) == 0b11000000)
				{
					switch ((instr[1] & 0b00111000) >> 3) // mode
					{
					case 0b000:
						instrExt();
						break;
					case 0b010:
					case 0b100:
					case 0b101:
					case 0b110:
					case 0b111:
						instrMovem(true); // reg to mem
						break;
					default:
						assert(0);
					}
				}
				else
					assert(0); /* instruction with 00 size bits goes here */
				break;
			}
			case 0b1010:
			{
				if ((instr[1] & 0b11000000) == 0b11000000) // size
				{
					assert(0); /* instruction without size bits goes here */
				}
				else
					instrTst();
				break;
			}
			case 0b1100:
			{
				if ((instr[1] & 0b11000000) == 0b10000000 ||
					(instr[1] & 0b11000000) == 0b11000000) // size
				{
					switch ((instr[1] & 0b00111000) >> 3) // mode
					{
					case 0b010:
					case 0b011:
					case 0b101:
					case 0b110:
					case 0b111:
						instrMovem(false); // mem to reg
						break;
					default:
						assert(0);
					}
				}
				else
					assert(0);
				break;
			}
			case 0b1110:
			{
				if (instr[1] == 0b01110101)
					instrRts();
				else if (instr[1] == 0b01110001)
					instrNop();
				else
				{
					if ((instr[1] & 0b11000000) == 0b11000000) // size
					{
						switch ((instr[1] & 0b00111000) >> 3) // mode
						{
						case 0b010:
						case 0b101:
						case 0b110:
						case 0b111:
							instrJmp();
							break;
						default:
							assert(0);
						}
					}
					else if ((instr[1] & 0b11000000) == 0b10000000) // size
					{
						switch ((instr[1] & 0b00111000) >> 3) // mode
						{
						case 0b010:
						case 0b101:
						case 0b110:
						case 0b111:
							instrJsr();
							break;
						default:
							assert(0);
						}
					}
					else
						assert(0); /* instr with 00/01 size bits goes here */
				}
				break;
			}
			default:
				assert(0);
			}
		}
		break;
	}
	case 0b0101: // ADDQ, SUBQ, Scc, DBcc, TRAPcc
	{
		/* this is a badly written special case for scc, since it puts its
		 * condition field right in the second nibble */
		if ((instr[1] & 0b11000000) == 0b11000000)
		{
			if (((instr[1] & 0b00111000) >> 3) != 0b001)
				instrScc();
			else
				assert(0); /* instruction with non-001 mode bits goes here */
		}
		/* another special case for addq, which has data in the second nibble */
		else if ((instr[0] & 0b00000001) == 0b00000000 &&
			(instr[1] & 0b11000000) != 0b11000000)
			instrAddq();
		else
			assert(0);
		break;
	}
	case 0b0110: // Bcc, BSR, BRA
	{
		switch (instr[0] & 0b1111)
		{
		case 0b0000:
			instrBra();
			break;
		case 0b0001:
			instrBsr();
			break;
		default:
			instrBcc();
			break;
		}
		break;
	}
	case 0b0111: // MOVEQ
	{
		if ((instr[0] & 0b00000001) == 0)
			instrMoveq();
		else
			assert(0);
		break;
	}
	case 0b1000: // OR, DIV, SBCD
	{
		if (((instr[1] & 0b00111000) >> 3) != 0b001) // mode
		{
			if ((instr[1] & 0b11000000) != 0b11000000) // size
			{
				if ((instr[0] & 0b00000001) == 0b00000000) // data reg dest
					instrEorOrAnd(true, false, true); // or
				else
					instrEorOrAnd(true, false, false); // or
			}
			else
				assert(0); /* instruction without size bits goes here */
		}
		else
			assert(0);
		break;
	}
	case 0b1001: // SUB, SUBX
	{
		assert(0);
		break;
	}
	case 0b1010: // reserved (used for emulator instructions)
	{
		instrEmu();
		break;
	}
	case 0b1011: // CMP, EOR
	{
		if (((instr[1] & 0b00111000) >> 3) != 0b001) // mode
		{
			if ((instr[1] & 0b11000000) != 0b11000000) // size
				instrEorOrAnd(true, true, false); // eor
			else
				assert(0); /* instruction without size bits goes here */
		}
		else
			assert(0);
		break;
	}
	case 0b1100: // AND, MUL, ABCD, EXG
	{
		switch ((instr[1] & 0b00111000) >> 3) // mode
		{
		case 0b000:
		case 0b001:
		{
			if ((instr[0] & 0b00000001) == 0b00000001)
			{
				switch (instr[1] & 0b11000000) // size
				{
				case 0b01000000:
				case 0b10000000:
					instrExg();
					break;
				default:
					assert(0); /* instruction with 00/11 size bits goes here */
				}
			}
			else
				assert(0);
			break;
		}
		default:
		{
			if ((instr[1] & 0b11000000) != 0b11000000) // size
			{
				if ((instr[0] & 0b00000001) == 0b00000000) // data reg dest
					instrEorOrAnd(false, false, true); // and
				else
					instrEorOrAnd(false, false, false); // and
			}
			else
				assert(0); /* instruction without size bits goes here */
			break;
		}
		}
		break;
	}
	case 0b1101: // ADD, ADDX
	{
		assert(0);
		break;
	}
	case 0b1110: // shift, rotate, bitfield
	{
		assert(0);
		break;
	}
	case 0b1111: // coprocessor interface, CPU32 extensions
	{
		assert(0);
		break;
	}
	}
	
	uartWritePSTR("[Done]\n");
}

void m68kDumpReg(void)
{
	uartWritePSTR("  pc: 0x");
	uartWriteHex32(cpu.ureg.pc.l, false);
	uartWritePSTR(" sr: 0x");
	uartWriteHex32(cpu.ureg.sr.l, false);
	uartWriteChr('\n');
	
	uartWritePSTR("  [");
	uartWriteChr((cpu.ureg.sr.l & SR_CARRY) ? 'x' : ' ');
	uartWritePSTR("] carry  [");
	uartWriteChr((cpu.ureg.sr.l & SR_OVERFLOW) ? 'x' : ' ');
	uartWritePSTR("] overflow  [");
	uartWriteChr((cpu.ureg.sr.l & SR_ZERO) ? 'x' : ' ');
	uartWritePSTR("] zero  [");
	uartWriteChr((cpu.ureg.sr.l & SR_NEGATIVE) ? 'x' : ' ');
	uartWritePSTR("] negative  [");
	uartWriteChr((cpu.ureg.sr.l & SR_EXTEND) ? 'x' : ' ');
	uartWritePSTR("] extend\n");
	
	uartWritePSTR("  d0: 0x");
	uartWriteHex32(cpu.ureg.d[0].l, false);
	uartWritePSTR(" d4: 0x");
	uartWriteHex32(cpu.ureg.d[4].l, false);
	uartWritePSTR(" a0: 0x");
	uartWriteHex32(cpu.ureg.a[0].l, false);
	uartWritePSTR(" a4: 0x");
	uartWriteHex32(cpu.ureg.a[4].l, false);
	uartWriteChr('\n');
	
	uartWritePSTR("  d1: 0x");
	uartWriteHex32(cpu.ureg.d[1].l, false);
	uartWritePSTR(" d5: 0x");
	uartWriteHex32(cpu.ureg.d[5].l, false);
	uartWritePSTR(" a1: 0x");
	uartWriteHex32(cpu.ureg.a[1].l, false);
	uartWritePSTR(" a5: 0x");
	uartWriteHex32(cpu.ureg.a[5].l, false);
	uartWriteChr('\n');
	
	uartWritePSTR("  d2: 0x");
	uartWriteHex32(cpu.ureg.d[2].l, false);
	uartWritePSTR(" d6: 0x");
	uartWriteHex32(cpu.ureg.d[6].l, false);
	uartWritePSTR(" a2: 0x");
	uartWriteHex32(cpu.ureg.a[2].l, false);
	uartWritePSTR(" bp: 0x");
	uartWriteHex32(cpu.ureg.a[6].l, false);
	uartWriteChr('\n');
	
	uartWritePSTR("  d3: 0x");
	uartWriteHex32(cpu.ureg.d[3].l, false);
	uartWritePSTR(" d7: 0x");
	uartWriteHex32(cpu.ureg.d[7].l, false);
	uartWritePSTR(" a3: 0x");
	uartWriteHex32(cpu.ureg.a[3].l, false);
	uartWritePSTR(" sp: 0x");
	uartWriteHex32(cpu.ureg.a[7].l, false);
	uartWriteChr('\n');
}

void m68kNext(void)
{
	/* fetch from EEPROM */
	m68kFetch();
	
	/* decode and execute */
	m68kDecode();
}

void m68kInit(void)
{
	/* TODO: figure out which registers to initialize */
	cpu.ureg.pc.l = 0x00000000;
	
	for (uint8_t i = 0; i < 8; ++i)
	{
		cpu.ureg.d[i].l = 0x00000000;
		cpu.ureg.a[i].l = 0x00000000;
	}
	
	/* start up in supervisor mode (but not master) */
	cpu.sreg.sr = SR_USERSTATE;
	
	/* debug */
	uartWritePSTR("CPU initialized.\n");
}
