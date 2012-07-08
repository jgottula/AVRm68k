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
	const uint32_t chunkSize = 0x8000;
	const uint16_t delayTime = 1000;
	
	uint16_t errors = 0;
	uint32_t min = 0xffffff, max = 0x000000;
	
	uartWritePSTR("dram test: 32KiB chunks, incr addr, alt aa/55, 1s wait, "
		"10us spacing...\n");
	for (uint32_t chunk = 0x000000; chunk < 0x1000000; chunk += chunkSize)
	{
		uint16_t chunkErrors = 0;
		
		uartWritePSTR("chunk 0x");
		uartWriteHex24(chunk, false);
		
		uartWritePSTR(": waiting");
		_delay_ms(delayTime);
		
		uartWritePSTR(" writing");
		
		for (uint32_t relAddr = 0x000000; relAddr < chunkSize; relAddr += 2)
		{
			uint32_t addr = chunk + relAddr;
			
			dramWrite(addr, 0xaa);
			_delay_us(10);
			dramWrite(addr + 1, 0x55);
			_delay_us(10);
		}
		
		uartWritePSTR(" waiting");
		_delay_ms(delayTime);
		
		uartWritePSTR(" reading");
		
		for (uint32_t relAddr = 0x000000; relAddr < chunkSize; relAddr += 2)
		{
			uint32_t addr = chunk + relAddr;
			
			if (dramRead(addr) != 0xaa)
			{
				if (addr < min)
					min = addr;
				if (addr > max)
					max = addr;
				++chunkErrors;
			}
			_delay_us(10);
			
			if (dramRead(addr + 1) != 0x55)
			{
				if (addr < min)
					min = addr;
				if (addr > max)
					max = addr;
				++chunkErrors;
			}
			_delay_us(10);
		}
		
		errors += chunkErrors;
		
		uartWritePSTR(" done (");
		uartWriteDec16(chunkErrors);
		uartWritePSTR(" errors).\n");
	}
	
	#if 0
	for (uint32_t i = 0xffffff; (int32_t)i >= 0; --i)
		dramWrite(i, i);
	_delay_ms(1000);
	for (uint32_t i = 0xffffff; (int32_t)i >= 0; --i)
	{
		uint8_t byte = dramRead(i);
		
		if (byte != (i & 0xff))
		{
			/*uartWritePSTR("failed @ 0x");
			uartWriteHex24(i, false);
			uartWritePSTR(" (expected: 0x");
			uartWriteHex8(i & 0xff, false);
			uartWritePSTR(" actual: 0x");
			uartWriteHex8(byte, false);
			uartWritePSTR(")\n");*/
			
			uartWriteHex24(i, false);
			uartWriteChr('\n');
			
			if (i < min)
				min = i;
			if (i > max)
				max = i;
			
			++errors;
		}
	}
	#endif
	
	uartWriteDec16(errors);
	uartWritePSTR(" total errors (min: 0x");
	uartWriteHex24(min, false);
	uartWritePSTR(", max: 0x");
	uartWriteHex24(max, false);
	uartWritePSTR(")\n");
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
