#include "test.h"
#include <util/delay.h>
#include "debug.h"
#include "dram.h"
#include "intr.h"
#include "m68k.h"
#include "m68k_mem.h"
#include "uart.h"

static void testDRAM(void)
{
	uint16_t errors = 0;
	
	uartWritePSTR("dram test: 16 MiB, incr pattern, 1000 ms:");
	for (uint32_t i = 0x000000; i <= 0xffffff; ++i)
		dramWrite(i, i);
	_delay_ms(1000);
	for (uint32_t i = 0x000000; i < 0xffffff; ++i)
	{
		uint8_t byte = dramRead(i);
		
		if (byte != (i & 0xff))
		{
			uartWritePSTR("failed @ 0x");
			uartWriteHex24(i, false);
			uartWritePSTR(" (expected: 0x");
			uartWriteHex8(i & 0xff, false);
			uartWritePSTR(" actual: 0x");
			uartWriteHex8(byte, false);
			uartWritePSTR(")\n");
			
			++errors;
		}
	}
	
	uartWriteDec16(errors);
	uartWritePSTR(" total errors.\n");
}

static void benchmarkDRAM(void)
{
	uint16_t before, after;
	
	before = msec;
	for (uint32_t i = 0x000000; i < 0x00ffff; ++i)
		dramRead(i);
	after = msec;
	
	uartWritePSTR("dram read speed: 64 KiB / ");
	uartWriteDec16((after - before) / 1000);
	uartWriteChr('.');
	uartWriteDec16((after - before) % 1000);
	uartWritePSTR(" s\n");
	
	before = msec;
	for (uint32_t i = 0x000000; i < 0x00ffff; ++i)
		dramWrite(i, 0);
	after = msec;
	
	uartWritePSTR("dram write speed: 64 KiB / ");
	uartWriteDec16((after - before) / 1000);
	uartWriteChr('.');
	uartWriteDec16((after - before) % 1000);
	uartWritePSTR(" s\n");
}

static void testRefresh(void)
{
	uint16_t t0 = msec;
	dramRefresh();
	uint16_t t1 = msec;
	uartWritePSTR("dram refresh duration: ");
	uartWriteDec16(t1 - t0);
	uartWritePSTR(" +/- 1 ms\n");
}

/* untested */
static void prePostIncrDecr(bool incr, uint8_t reg, uint8_t size)
{
	uint8_t actualSize;
	
	switch (size)
	{
	case SIZE_BYTE:
		if (reg == 7) // special case for stack pointer
			actualSize = 2;
		else
			actualSize = 1;
		break;
	case SIZE_WORD:
		actualSize = 2;
		break;
	case SIZE_LONG:
		actualSize = 4;
		break;
	default:
		assert(0);
	}
	
	if (incr)
		cpu.ureg.a[reg].l += actualSize;
	else
		cpu.ureg.a[reg].l -= actualSize;
}

void testAll(void)
{
	uartWritePSTR("-------- Unit Tests --------\n");
	
	benchmarkDRAM();
	testRefresh();
	testDRAM();
	
	/*m68kDumpReg();
	prePostIncrDecr(true, 6, SIZE_BYTE);
	m68kDumpReg();
	prePostIncrDecr(true, 6, SIZE_WORD);
	m68kDumpReg();
	prePostIncrDecr(true, 6, SIZE_LONG);
	m68kDumpReg();
	prePostIncrDecr(false, 6, SIZE_BYTE);
	m68kDumpReg();
	prePostIncrDecr(false, 6, SIZE_WORD);
	m68kDumpReg();
	prePostIncrDecr(false, 6, SIZE_LONG);
	m68kDumpReg();
	prePostIncrDecr(true, 7, SIZE_BYTE);
	m68kDumpReg();
	prePostIncrDecr(false, 7, SIZE_BYTE);
	m68kDumpReg();*/
	
	/*writeReg(0b000, 0, 0x12345678, SIZE_LONG);
	writeReg(0b000, 1, 0x1234, SIZE_WORD);
	writeReg(0b000, 2, 0x12, SIZE_BYTE);
	writeReg(0b001, 0, 0x87654321, SIZE_LONG);
	writeReg(0b001, 1, 0x4321, SIZE_WORD);
	writeReg(0b001, 2, 0x21, SIZE_BYTE);
	m68kDumpReg();*/
	
	/*cpu.ureg.d[0].l = 0xdeadbeef;
	cpu.ureg.a[0].l = 0xcafebabe;
	
	uartWriteHex8(cpu.ureg.d[0].b[0], false); uartWriteChr('\n');
	uartWriteHex8(cpu.ureg.d[0].b[1], false); uartWriteChr('\n');
	uartWriteHex8(cpu.ureg.d[0].b[2], false); uartWriteChr('\n');
	uartWriteHex8(cpu.ureg.d[0].b[3], false); uartWriteChr('\n');
	
	uartWriteHex8(readReg(0b000, 0, SIZE_BYTE), false); uartWriteChr('\n');
	uartWriteHex16(readReg(0b000, 0, SIZE_WORD), false); uartWriteChr('\n');
	uartWriteHex32(readReg(0b000, 0, SIZE_LONG), false); uartWriteChr('\n');
	uartWriteHex8(readReg(0b001, 0, SIZE_BYTE), false); uartWriteChr('\n');
	uartWriteHex16(readReg(0b001, 0, SIZE_WORD), false); uartWriteChr('\n');
	uartWriteHex32(readReg(0b001, 0, SIZE_LONG), false); uartWriteChr('\n');*/
	
	uartWritePSTR("------ Tests Complete ------\n");
}
