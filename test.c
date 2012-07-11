#include "test.h"
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "debug.h"
#include "dram.h"
#include "intr.h"
#include "m68k.h"
#include "m68k_mem.h"
#include "sram.h"
#include "uart.h"

static void testSRAM(void)
{
	bool fail = false;
	
	uartWritePSTR("sram byte mode: ");
	sramWriteByte(0x100, 0x55);
	if (sramReadByte(0x100) != 0x55)
		fail = true;
	sramWriteByte(0x100, 0xaa);
	if (sramReadByte(0x100) != 0xaa)
		fail = true;
	
	if (fail)
		uartWritePSTR("failed\n");
	else
		uartWritePSTR("passed\n");
	
	uartWritePSTR("sram seq mode: ");
	const uint8_t src[] = "hello world";
	uint8_t dest[0x10];
	sramWriteSeq(0x100, 12, src);
	sramReadSeq(0x100, 12, dest);
	
	if (memcmp(src, dest, 12) != 0)
		uartWritePSTR("failed\n");
	else
		uartWritePSTR("passed\n");
}

static void benchmarkSRAM(void)
{
	uint16_t before, after;
	
	before = msec;
	for (uint32_t i = 0x0000; i < 0x0400; ++i)
		sramReadByte(i);
	after = msec;
	
	uartWritePSTR("sram byte read speed: 1024 B / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
	
	before = msec;
	uint8_t dest[0x400];
	sramReadSeq(0x0000, 0x400, dest);
	after = msec;
	
	uartWritePSTR("sram seq read speed: 1024 B / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
}

static void testDRAM(void)
{
	const uint32_t chunkSize = 0x8000;
	const uint16_t delayTime = 100;
	
	uint16_t errors = 0;
	uint32_t min = 0xffffff, max = 0x000000;
	
	/*for (uint32_t addr = 0x000000; addr < DRAM_SIZE; addr += 0x10)
	{
		uartWritePSTR("\n0x");
		uartWriteHex24(addr, false);
		uartWriteChr(':');
		
		for (uint8_t off = 0; off < 0x10; ++off)
		{
			uartWriteChr(' ');
			uartWriteHex8(dramRead(addr + off), false);
		}
	}*/
	
	/*for (uint32_t addr = 0x03f000; addr < 0x03f800; addr += 0x100)
	{
		uint8_t val = 0;
		
		uartWritePSTR("\n0x");
		uartWriteHex24(addr, false);
		
		do
		{
			dramWrite(addr, val);
			uint8_t byte = dramRead(addr);
			
			if (byte == val)
			{
				uartWriteChr(' ');
				uartWriteHex8(val, false);
			}
		}
		while (--val != 0);
	}*/
	
	uartWritePSTR("dram test: 32KiB chunks, incr addr, pat 55/aa, 100ms wait, "
		"1us spacing...\n");
	for (uint32_t chunk = 0x000000; chunk < 0x1000000; chunk += chunkSize)
	{
		uint16_t chunkErrors;
		
	redo:
		chunkErrors = 0;
		
		uartWritePSTR("chunk 0x");
		uartWriteHex24(chunk, false);
		
		uartWritePSTR(": waiting");
		_delay_ms(delayTime);
		
		uartWritePSTR(" writing");
		
		for (uint32_t relAddr = 0x000000; relAddr < chunkSize; relAddr += 2)
		{
			uint32_t addr = chunk + relAddr;
			
			dramWrite(addr, 0x55);
			_delay_us(1);
			dramWrite(addr + 1, 0xaa);
			_delay_us(1);
		}
		
		uartWritePSTR(" waiting");
		_delay_ms(delayTime);
		
		uartWritePSTR(" reading");
		
		for (uint32_t relAddr = 0x000000; relAddr < chunkSize; relAddr += 2)
		{
			uint32_t addr = chunk + relAddr;
			uint8_t byte;
			
			if ((byte = dramRead(addr)) != 0x55)
			{
				if (addr < min)
					min = addr;
				if (addr > max)
					max = addr;
				++chunkErrors;
				
				/*uartWritePSTR("\nerror @ 0x");
				uartWriteHex24(addr, false);
				uartWritePSTR(": exp 0x");
				uartWriteHex8(0x55, true);
				uartWritePSTR(" act 0x");
				uartWriteHex8(byte, true);
				uartWriteChr(' ');*/
			}
			_delay_us(1);
			
			if ((byte = dramRead(addr + 1)) != 0xaa)
			{
				if (addr < min)
					min = addr;
				if (addr > max)
					max = addr;
				++chunkErrors;
				
				/*uartWritePSTR("\nerror @ 0x");
				uartWriteHex24(addr + 1, false);
				uartWritePSTR(": exp 0x");
				uartWriteHex8(0xaa, true);
				uartWritePSTR(" act 0x");
				uartWriteHex8(byte, true);
				uartWriteChr(' ');*/
			}
			_delay_us(1);
		}
		
		errors += chunkErrors;
		
		uartWritePSTR(" done (");
		uartWriteDec16(chunkErrors);
		uartWritePSTR(" errors).\n");
		
		if (chunkErrors > 0)
			goto redo;
	}
	
#if 0
	for (uint32_t i = 0xffffff; (int32_t)i >= 0; --i)
		dramWrite(i, 0xff);
	_delay_ms(1000);
	for (uint32_t i = 0xffffff; (int32_t)i >= 0; --i)
	{
		uint8_t byte = dramRead(i);
		
		if (byte != 0xff)
		{
			uartWritePSTR("failed @ 0x");
			uartWriteHex24(i, false);
			uartWritePSTR(" (expected: 0x");
			uartWriteHex8(0xff, false);
			uartWritePSTR(" actual: 0x");
			uartWriteHex8(byte, false);
			uartWritePSTR(")\n");
			
			/*uartWriteHex24(i, false);
			uartWriteChr('\n');*/
			
			if (i < min)
				min = i;
			if (i > max)
				max = i;
			
			++errors;
		}
	}
	
	uartWriteDec16(errors);
	uartWritePSTR(" total errors (min: 0x");
	uartWriteHex24(min, false);
	uartWritePSTR(", max: 0x");
	uartWriteHex24(max, false);
	uartWritePSTR(")\n");
#endif
}

static void benchmarkDRAM(void)
{
	uint16_t before, after;
	
	before = msec;
	for (uint32_t i = 0x000000; i < 0x00ffff; ++i)
		dramRead(i);
	after = msec;
	
	uartWritePSTR("dram read speed: 64 KiB / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
	
	before = msec;
	for (uint32_t i = 0x000000; i < 0x00ffff; ++i)
		dramWrite(i, 0);
	after = msec;
	
	uartWritePSTR("dram write speed: 64 KiB / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
}

static void testRefresh(void)
{
	/* protect from competing ISR refreshes */
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		uint16_t t0 = msec;
		dramRefresh();
		uint16_t t1 = msec;
		uartWritePSTR("dram refresh duration: ");
		uartWriteDec16(t1 - t0);
		uartWritePSTR(" +/- 1 ms\n");
	}
}

void testAll(void)
{
	uartWritePSTR("-------- Unit Tests --------\n");
	
	/* write to ram */
	for (uint32_t addr = 0x000000; addr < 0x000100; ++addr)
		dramWrite(addr, 0xaa);
	
	testSRAM();
	benchmarkSRAM();
	benchmarkDRAM();
	testRefresh();
	testDRAM();
	
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
