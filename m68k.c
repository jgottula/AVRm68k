#include "m68k.h"
#include <avr/eeprom.h>
#include "debug.h"
#include "intr.h"
#include "m68k_instr.h"
#include "m68k_mem.h"
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
	
	uint16_t before = msec;
	
	/* separate operations by front nibble */
	switch (instr[0] >> 4)
	{
	case 0b0000: // bit manipulation, MOVEP, immediate
	{
		/* special case for btst, as it puts a reg field in the second nibble */
		if (((instr[1] & 0b11000000) == 0b00000000 ||
			(instr[1] & 0b11000000) == 0b11000000) && // size
			(instr[1] & 0b00111000) != 0b00001000) // mode
		{
			if ((instr[1] & 0b11000000) == 0b00000000)
			{
				if ((instr[0] & 0b00000001) == 0b0000001)
					instrBsetBtst(false, false); // register
				else
					instrBsetBtst(false, true); // immediate
			}
			else if ((instr[1] & 0b11000000) == 0b11000000)
			{
				if ((instr[0] & 0b00000001) == 0b0000001)
					instrBsetBtst(true, false); // register
				else
					instrBsetBtst(true, true); // immediate
			}
		}
		else
		{
			switch (instr[0] & 0b1111)
			{
			case 0b0000:
				if ((instr[1] & 0b11000000) != 0b11000000) // size
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrAndiEoriOri(true, false); // ori
					else
						assert(0);
				}
				else
					assert(0); /* instruction without size bits goes here */
				break;
			case 0b1010:
				if ((instr[1] & 0b11000000) != 0b11000000) // size
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrAndiEoriOri(true, true); // eori
					else
						assert(0);
				}
				else
					assert(0); /* instruction without size bits goes here */
				break;
			case 0b0010:
				if ((instr[1] & 0b11000000) != 0b11000000) // size
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrAndiEoriOri(false, false); // andi
					else
						assert(0);
				}
				else
					assert(0); /* instruction without size bits goes here */
				break;
			default:
				assert(0);
			}
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
			(instr[1] & 0b11000000) == 0b11000000) // size
		{
			switch ((instr[1] & 0b00111000) >> 3) // mode
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
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrNeg(true); // with extend
					else
						assert(0);
				}
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
				{
					if ((instr[1] & 0b00111000) != 0b00001000) // mode
						instrNeg(false); // without extend
					else
						assert(0);
				}
				break;
			}
			case 0b0110:
			{
				if ((instr[1] & 0b11000000) == 0b11000000) // size
					instrMoveToSr();
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
					(instr[1] & 0b11000000) == 0b11000000) // size
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
				else // size
				{
					switch ((instr[1] & 0b00111000) >> 3) // mode
					{
					case 0b001:
						instrLink(SIZE_LONG);
						break;
					default:
						assert(0);
					}
				}
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
					else if ((instr[1] & 0b11000000) == 0b01000000) // size
					{
						switch ((instr[1] & 0b00111000) >> 3) // mode
						{
						case 0b010:
							instrLink(SIZE_WORD);
							break;
						case 0b011:
							instrUnlk();
							break;
						case 0b100:
							instrMoveUsp(true); // an to usp
							break;
						case 0b101:
							instrMoveUsp(false); // usp to an
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
						assert(0); /* instr with 00 size bits goes here */
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
		if ((instr[1] & 0b11000000) == 0b11000000) // size
		{
			if (((instr[1] & 0b00111000) >> 3) != 0b001) // mode
				instrScc();
			else
				instrDbcc();
		}
		else if ((instr[0] & 0b00000001) == 0b00000000)
		{
			if ((instr[1] & 0b11000000) != 0b11000000)
				instrAddq();
			else
				assert(0);
		}
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
					instrAndEorOr(true, false, true); // or
				else
					instrAndEorOr(true, false, false); // or
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
				instrAndEorOr(true, true, false); // eor
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
					instrAndEorOr(false, false, true); // and
				else
					instrAndEorOr(false, false, false); // and
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
		switch ((instr[1] & 0b00111000) >> 3) // mode
		{
		case 0b000:
			instrAslAsrReg(false,
				((instr[1] & 0b00100000) != 0b00100000)); // right shift
			break;
		case 0b100:
			instrAslAsrReg(true,
				((instr[1] & 0b00100000) != 0b00100000)); // left shift
			break;
		default:
			assert(0);
		}
		break;
	}
	case 0b1111: // coprocessor interface, CPU32 extensions
	{
		assert(0);
		break;
	}
	}
	
	uint16_t after = msec;
	
	uartWritePSTR("[Done ");
	uartWriteDec16(after - before);
	uartWritePSTR("ms]\n");
}

void m68kDumpReg(void)
{
	uartWritePSTR("  pc: 0x");
	uartWriteHex32(cpu.ureg.pc.l, false);
	uartWritePSTR(" sr: 0x");
	uartWriteHex16(cpu.sreg.sr.w, false);
	uartWriteChr('\n');
	
	uartWritePSTR("  [");
	uartWriteChr((cpu.sreg.sr.w & SR_USERSTATE) ? 'x' : ' ');
	uartWritePSTR("] supervisor\n");
	
	uartWritePSTR("  [");
	uartWriteChr((cpu.sreg.sr.b[0] & SR_CARRY) ? 'x' : ' ');
	uartWritePSTR("] carry  [");
	uartWriteChr((cpu.sreg.sr.b[0] & SR_OVERFLOW) ? 'x' : ' ');
	uartWritePSTR("] overflow  [");
	uartWriteChr((cpu.sreg.sr.b[0] & SR_ZERO) ? 'x' : ' ');
	uartWritePSTR("] zero  [");
	uartWriteChr((cpu.sreg.sr.b[0] & SR_NEGATIVE) ? 'x' : ' ');
	uartWritePSTR("] negative  [");
	uartWriteChr((cpu.sreg.sr.b[0] & SR_EXTEND) ? 'x' : ' ');
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
	cpu.ureg.pc.l = 0x00000000;
	
	for (uint8_t i = 0; i < 8; ++i)
	{
		cpu.ureg.d[i].l = 0x00000000;
		cpu.ureg.a[i].l = 0x00000000;
	}
	
	/* start up in supervisor mode (but not master) */
	cpu.sreg.sr.w = SR_USERSTATE;
	
	/* vectors: initial ssp; initial pc */
	memWrite(0x00000000, SIZE_LONG, 0x01000000);
	memWrite(0x00000004, SIZE_LONG, 0x00000000); // currently maps to eeprom:0
	
	/* debug */
	uartWritePSTR("CPU initialized.\n");
}
