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

static void m68kExecute(void)
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
				assert(0);
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
				assert(0);
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
				assert(0);
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
		#warning TODO: reorder instr nibble 0b0100
		
		switch (instr[0] & 0b1111)
		{
		case 0b0000:
		{
			if ((instr[1] & 0b11000000) == 0b11000000) // size
			{
				if ((instr[1] & 0b00111000) != 0b00001000) // mode
					instrMoveFromSr();
				else
					assert(0); /* instruction with size bits goes here */
			}
			else
				assert(0);
			break;
		}
		case 0b0010:
		{
			if ((instr[1] & 0b11000000) == 0b11000000) // size
			{
				if ((instr[1] & 0b00111000) != 0b00001000) // mode
					instrMoveFromCcr();
				else
					instrClr();
			}
			else
				assert(0);
			break;
		}
		case 0b0100:
		{
			if ((instr[1] & 0b11000000) == 0b11000000) // size
			{
				if ((instr[1] & 0b00111000) != 0b00001000) // mode
					instrMoveToCcr();
				else
					assert(0); /* instruction with size bits goes here */
			}
			else
				assert(0);
			break;
		}
		case 0b1000:
		{
			if ((instr[1] & 0b11000000) == 0b01000000) // size
			{
				switch (instr[1] & 0b00111000) // mode
				{
				case 0b010:
				case 0b101:
				case 0b110:
				case 0b111:
					instrPea();
					break;
				default:
					assert(0);
				}
			}
			else
				assert(0);
		}
		case 0b1110:
		{
			if (instr[1] == 0x71)
				instrNop();
			else
				assert(0);
			break;
		}
		default:
			assert(0);
		}
		
		/*
		else if (instr[0] == 0x48 && ((instr[1] & 0b11000000) == 0b01000000) &&
			((instr[1] & 0b00111000) != 0b00000000))
			instrPea();
		else if (instr[0] == 0x46)
			instrNot();
		else if (instr[0] == 0x4e && ((instr[1] & 0b11000000) == 0b11000000))
			instrJmp();
		else if (instr[0] == 0x4e && ((instr[1] & 0b11000000) == 0b10000000))
			instrJsr();
		else if (instr[0] == 0x4e && instr[1] == 0x75)
			instrRts();
		else if (instr[0] == 0x4a)
			instrTst();
		else if (((instr[0] & 0b11111011) == 0b01001000) &&
			((instr[1] & 0b10000000) == 0b10000000))
			instrMovem();
		else if (instr[0] == 0x48 && ((instr[1] & 0b11111000) == 0b001000000))
			instrSwap();
		else if (((instr[0] & 0b11111110) == 0b01001000) &&
			((instr[1] & 0b00111000) == 0b00000000))
			instrExt();
		else
			assert(0);*/
		break;
	}
	case 0b0101: // ADDQ, SUBQ, Scc, DBcc, TRAPcc
	{
		if ((instr[1] & 0b11000000) == 0b11000000)
			instrScc();
		else if ((instr[0] & 0b00000001) == 0b00000000)
			instrAddq();
		else
			assert(0);
		break;
	}
	case 0b0110: // Bcc, BSR, BRA
	{
		if (instr[0] == 0x60)
			instrBra();
		else if (instr[0] == 0x61)
			instrBsr();
		else
			instrBcc();
		break;
	}
	case 0b0111: // MOVEQ
	{
		if ((instr[0] & 0b1) == 0)
			instrMoveq();
		else
			assert(0);
		break;
	}
	case 0b1000: // OR, DIV, SBCD
	{
		if (((instr[0] & 0b00000001) == 0b00000000) ||
			(((instr[1] >> 3) & 0b00000111) > 0b001))
			instrEorOrAnd(true, false, ((instr[0] & 0b00000001) == 0));
		else
			assert(0);
		break;
	}
	case 0b1001: // SUB, SUBX
	{
		assert(0);
		break;
	}
	case 0b1010: // reserved (emulator instructions)
	{
		assert(instrEmu());
		break;
	}
	case 0b1011: // CMP, EOR
	{
		if ((instr[0] & 0b00000001) == 0b00000001)
			instrEorOrAnd(true, true, false);
		else
			assert(0);
		break;
	}
	case 0b1100: // AND, MUL, ABCD, EXG
	{
		if (((instr[0] & 0b00000001) == 0b00000001) &&
			((instr[1] & 0b00110000) == 0b00000000) &&
			(((instr[1] >> 3) & 0b00000111) <= 0b001))
			instrExg();
		else if (((instr[0] & 0b00000001) == 0b00000000) &&
			((instr[1] & 0b11000000) == 0b11000000))
		{
			assert(0); // mulu
		}
		else if (((instr[0] & 0b00000001) == 0b00000001) &&
			((instr[1] >> 4) == 0b0000))
		{
			assert(0); // abcd
		}
		else
			instrEorOrAnd(false, false, ((instr[0] & 0b00000001) == 0));
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
	
	/* execute */
	m68kExecute();
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
	
	/* debug */
	uartWritePSTR("CPU initialized.\n");
}
