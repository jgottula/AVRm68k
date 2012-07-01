#include "m68k.h"
#include <avr/eeprom.h>
#include "debug.h"
#include "m68k_instr.h"
#include "uart.h"

#if 0

uint8_t fetchBuffer[0x10];
uint8_t *instr;
uint8_t prefix;

static void i386Fetch(void)
{
	eeprom_read_block(fetchBuffer, (const void *)(intptr_t)cpu.reg.eip.dword,
		sizeof(fetchBuffer));
	
	/* debug */
	dbgHeader();
	uartWritePSTR("Fetch:");
	for (uint8_t i = 0; i < sizeof(fetchBuffer); ++i)
	{
		uartWriteChr(' ');
		uartWriteHex8(fetchBuffer[i], false);
	}
	uartWriteChr('\n');
}

static void i386Execute(void)
{
	uint32_t eipBefore = cpu.reg.eip.dword;
	
	instr = fetchBuffer;
	
	/* detect prefixes and advance the instr pointer as needed */
	prefix = 0;
	if (instr[0] == 0x66)
	{
		prefix |= PFX_OPSIZE;
		++instr;
		
		dbgHeader();
		uartWritePSTR("PFX_OPSIZE\n");
	}
	if (instr[0] == 0x67)
	{
		prefix |= PFX_ADDRSIZE;
		++instr;
		
		dbgHeader();
		uartWritePSTR("PFX_ADDRSIZE\n");
	}
	
	/* in the future, a jump table may be faster (but larger) */
	if ((instr[0] >= 0x88 && instr[0] <= 0x8d) ||
		(instr[0] >= 0xa0 && instr[0] <= 0xa3) ||
		(instr[0] >= 0xb0 && instr[0] <= 0xbf) ||
		(instr[0] >= 0xc6 && instr[0] <= 0x67))
		instrMOV();
	else if ((instr[0] >= 0x30 && instr[0] <= 0x35) ||
		(instr[0] >= 0x80 && instr[0] <= 0x83))
		instrXOR();
	else if (instr[0] == 0x86 || instr[0] == 0x87 ||
		(instr[0] >= 0x91 && instr[0] <= 0x97))
		instrXCHG();
	else if (instr[0] == 0x06 || instr[0] == 0x0e ||
		instr[0] == 0x16 || instr[0] == 0x1e ||
		(instr[0] >= 0x50 && instr[0] <= 0x57) ||
		instr[0] == 0x68 || instr[0] == 0x6a ||
		instr[0] == 0xff)
		instrPUSH();
	else if (instr[0] == 0x07 || instr[0] == 0x17 ||
		instr[0] == 0x1f || instr[0] == 0x8f ||
		(instr[0] >= 0x58 && instr[0] <= 0x5f))
		instrPOP();
	else if (instr[0] == 0x90)
		instrNOP();
	else if (instr[0] >= 0x40 && instr[0] <= 0x47)
		instrIncReg();
	else if (instr[0] >= 0x48 && instr[0] <= 0x4f)
		instrDecReg();
	else if (instr[0] == 0x0f) // two-byte opcode
	{
		if (instr[1] == 0xa0 || instr[1] == 0xa8)
			instrPUSH();
		else if (instr[1] == 0xa1 || instr[1] == 0xa9)
			instrPOP();
		else
			assert(0);
	}
	else
		assert(0);
	
	/* ensure that the instruction pointer was advanced */
	/* NOTE: this will give false positives on tight loops */
	assert(cpu.reg.eip.dword != eipBefore);
}

static void i386DumpReg(void)
{
	dbgHeader();
	uartWritePSTR("----------\n");
	dbgHeader();
	uartWritePSTR("CPU STATE:\n");
	
	dbgHeader();
	uartWritePSTR("eip: 0x");
	uartWriteHex32(cpu.reg.eip.dword, false);
	uartWritePSTR(" eflags: 0x");
	uartWriteHex32(cpu.reg.eflags.dword, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWriteChr('[');
	uartWriteChr((cpu.reg.eflags.dword & FLAG_CF) ? 'x' : ' ');
	uartWritePSTR("] carry  [");
	uartWriteChr((cpu.reg.eflags.dword & FLAG_PF) ? 'x' : ' ');
	uartWritePSTR("] parity  [");
	uartWriteChr((cpu.reg.eflags.dword & FLAG_ZF) ? 'x' : ' ');
	uartWritePSTR("] zero  [");
	uartWriteChr((cpu.reg.eflags.dword & FLAG_SF) ? 'x' : ' ');
	uartWritePSTR("] sign  [");
	uartWriteChr((cpu.reg.eflags.dword & FLAG_OF) ? 'x' : ' ');
	uartWritePSTR("] overflow\n");
	
	dbgHeader();
	uartWritePSTR("eax: 0x");
	uartWriteHex32(cpu.reg.eax.dword, false);
	uartWritePSTR(" ebx: 0x");
	uartWriteHex32(cpu.reg.ebx.dword, false);
	uartWritePSTR(" ecx: 0x");
	uartWriteHex32(cpu.reg.ecx.dword, false);
	uartWritePSTR(" edx: 0x");
	uartWriteHex32(cpu.reg.edx.dword, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWritePSTR("ebp: 0x");
	uartWriteHex32(cpu.reg.ebp.dword, false);
	uartWritePSTR(" esp: 0x");
	uartWriteHex32(cpu.reg.esp.dword, false);
	uartWritePSTR(" esi: 0x");
	uartWriteHex32(cpu.reg.esi.dword, false);
	uartWritePSTR(" edi: 0x");
	uartWriteHex32(cpu.reg.edi.dword, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWritePSTR("cs: 0x");
	uartWriteHex16(cpu.reg.cs.word, false);
	uartWritePSTR(" ss: 0x");
	uartWriteHex16(cpu.reg.ss.word, false);
	uartWritePSTR(" ds: 0x");
	uartWriteHex16(cpu.reg.ds.word, false);
	uartWritePSTR(" es: 0x");
	uartWriteHex16(cpu.reg.es.word, false);
	uartWritePSTR(" fs: 0x");
	uartWriteHex16(cpu.reg.fs.word, false);
	uartWritePSTR(" gs: 0x");
	uartWriteHex16(cpu.reg.gs.word, false);
	uartWriteChr('\n');
	
	dbgHeader();
	uartWritePSTR("----------\n");
}

void i386Next(void)
{
	/* debug */
	i386DumpReg();
	
	/* fetch from EEPROM */
	i386Fetch();
	
	/* execute */
	i386Execute();
}

void i386Init(void)
{
	cpu.reg.eax.dword = 0x00000000; // passed POST
	
	cpu.reg.edx.byte[1] = 3; // device ID
	cpu.reg.edx.byte[0] = 8; // stepping ID (Ex/Fx, the most recent)
	
	cpu.ctrlReg.cr0.dword = 0x00000000;
	
	cpu.reg.eflags.dword = 0x00000002;
	cpu.reg.eip.dword = 0xfffffff0;
	cpu.reg.esp.dword = 0x00000000; // this needs to be in RAM
	
	/* TODO: first far jump/call will zero out the MSword of eip */
	
	cpu.reg.cs.word = 0x0000;
	cpu.reg.ds.word = 0x0000;
	cpu.reg.es.word = 0x0000;
	cpu.reg.ss.word = 0x0000;
	cpu.reg.fs.word = 0x0000;
	cpu.reg.gs.word = 0x0000;
	
	/* TODO: set mmu registers to their initial values; see p174 of manual */
	
	/* debug */
	dbgHeader();
	uartWritePSTR("CPU initialized.\n");
}

#endif
