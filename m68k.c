#include "m68k.h"
#include <avr/eeprom.h>
#include "debug.h"
#include "m68k_instr.h"
#include "uart.h"

uint8_t fetchBuffer[22]; // must have even size
const uint8_t *instr;
const uint16_t *instrWord;

static void m68kFetch(void)
{
	eeprom_read_block(fetchBuffer, (const void *)(intptr_t)cpu.ureg.pc.l,
		sizeof(fetchBuffer));
	
	/* debug */
	dbgHeader();
	uartWritePSTR("Fetch:");
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
	instrWord = (const uint16_t *)instr;
	
	/* separate operations by front nibble */
	switch (instr[0] >> 4)
	{
	case 0b0000: // bit manipulation, MOVEP, immediate
		assert(0);
		break;
	case 0b0001: // move byte
		assert(0);
		break;
	case 0b0010: // move long
		assert(0);
		break;
	case 0b0011: // move word
		assert(0);
		break;
	case 0b0100: // miscellaneous
		if (*instrWord == 0x714e)
			instrNop();
		else if ((instr[0] == 0x42) && ((instr[1] & 0b11000000) == 0b11000000))
			instrMoveCcr();
		else
			assert(0);
		break;
	case 0b0101: // ADDQ, SUBQ, Scc, DBcc, TRAPcc
		assert(0);
		break;
	case 0b0110: // Bcc, BSR, BRA
		assert(0);
		break;
	case 0b0111: // MOVEQ
		assert(0);
		break;
	case 0b1000: // OR, DIV, SBCD
		assert(0);
		break;
	case 0b1001: // SUB, SUBX
		assert(0);
		break;
	case 0b1010: // reserved
		assert(0);
		break;
	case 0b1011: // CMP, EOR
		assert(0);
		break;
	case 0b1100: // AND, MUL, ABCD, EXG
		if (instr[0] & 0b00000001)
			instrExg();
		else
			assert(0);
		break;
	case 0b1101: // ADD, ADDX
		assert(0);
		break;
	case 0b1110: // shift, rotate, bitfield
		assert(0);
		break;
	case 0b1111: // coprocessor interface, CPU32 extensions
		assert(0);
		break;
	}
}

static void m68kDumpReg(void)
{
	dbgHeader();
	uartWritePSTR("----------\n");
	
	dbgHeader();
	uartWritePSTR("pc: 0x");
	uartWriteHex32(cpu.ureg.pc.l, false);
	uartWritePSTR(" sr: 0x");
	uartWriteHex32(cpu.ureg.sr.l, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWriteChr('[');
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
	
	dbgHeader();
	uartWritePSTR("d0: 0x");
	uartWriteHex32(cpu.ureg.d[0].l, false);
	uartWritePSTR(" d4: 0x");
	uartWriteHex32(cpu.ureg.d[4].l, false);
	uartWritePSTR(" a0: 0x");
	uartWriteHex32(cpu.ureg.a[0].l, false);
	uartWritePSTR(" a4: 0x");
	uartWriteHex32(cpu.ureg.a[4].l, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWritePSTR("d1: 0x");
	uartWriteHex32(cpu.ureg.d[1].l, false);
	uartWritePSTR(" d5: 0x");
	uartWriteHex32(cpu.ureg.d[5].l, false);
	uartWritePSTR(" a1: 0x");
	uartWriteHex32(cpu.ureg.a[1].l, false);
	uartWritePSTR(" a5: 0x");
	uartWriteHex32(cpu.ureg.a[5].l, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWritePSTR("d2: 0x");
	uartWriteHex32(cpu.ureg.d[2].l, false);
	uartWritePSTR(" d6: 0x");
	uartWriteHex32(cpu.ureg.d[6].l, false);
	uartWritePSTR(" a2: 0x");
	uartWriteHex32(cpu.ureg.a[2].l, false);
	uartWritePSTR(" a6: 0x");
	uartWriteHex32(cpu.ureg.a[6].l, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWritePSTR("d3: 0x");
	uartWriteHex32(cpu.ureg.d[3].l, false);
	uartWritePSTR(" d7: 0x");
	uartWriteHex32(cpu.ureg.d[7].l, false);
	uartWritePSTR(" a3: 0x");
	uartWriteHex32(cpu.ureg.a[3].l, false);
	uartWritePSTR(" a7: 0x");
	uartWriteHex32(cpu.ureg.a[7].l, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWritePSTR("----------\n");
}

void m68kNext(void)
{
	/* debug */
	m68kDumpReg();
	
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
		cpu.ureg.d[i].l = 0x11111111 * (7 - i);
		cpu.ureg.a[i].l = 0x11111111 * (15 - i);
	}
	
	/* debug */
	dbgHeader();
	uartWritePSTR("CPU initialized.\n");
}
