#include "test.h"
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "debug.h"
#include "dram.h"
#include "io.h"
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
	for (uint32_t i = 0x0000; i < 0x1000; ++i)
		sramReadByte(i);
	after = msec;
	
	uartWritePSTR("sram byte read speed: 4 KiB / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
	
	before = msec;
	uint8_t dest[1024];
	sramReadSeq(0x0000, sizeof(dest), dest);
	sramReadSeq(0x0000, sizeof(dest), dest);
	sramReadSeq(0x0000, sizeof(dest), dest);
	sramReadSeq(0x0000, sizeof(dest), dest);
	after = msec;
	
	uartWritePSTR("sram seq read speed: (x4) 4 KiB / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
}

static void testDRAM(void)
{
#if 0
	const uint32_t chunkSize = 0x8000;
	const uint16_t delayTime = 100;
	
	uint16_t errors = 0;
	uint32_t min = 0xffffff, max = 0x000000;
	
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
	
	uartWritePSTR("writing.\n");
	for (uint32_t i = 0x000000; i < DRAM_SIZE / 8; i += 256)
	{
		uint8_t arr[256];
		
		for (uint16_t j = 0; j < 256; ++j)
			arr[j] = j;
		
		dramWriteFPM(i, 256, arr);
	}
	uartWritePSTR("done writing.\n");
	/*uartWritePSTR("3 minutes left.\n");
	_delay_ms(60000);
	uartWritePSTR("2 minutes left.\n");
	_delay_ms(60000);
	uartWritePSTR("1 minute left.\n");
	_delay_ms(60000);*/
	uartWritePSTR("reading.\n");
	for (uint32_t i = 0x000000; i < DRAM_SIZE / 8; i += 256)
	{
		uint8_t arr[256];
		
		dramReadFPM(i, 256, arr);
		
		for (uint16_t j = 0; j < 256; ++j)
		{
			if (arr[j] != (j & 0xff))
			{
				uartWritePSTR("err: 0x");
				uartWriteHex24(i, false);
				uartWriteChr('\n');
			}
		}

	}
	uartWritePSTR("done.\n");
}

static void benchmarkDRAM(void)
{
	uint16_t before, after;
	
	before = msec;
	for (uint32_t i = 0x000000; i < 0x001000; ++i)
		dramRead(i);
	after = msec;
	
	uartWritePSTR("dram read speed: 4 KiB / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
	
	before = msec;
	for (uint32_t i = 0x000000; i < 0x001000; ++i)
		dramWrite(i, 0);
	after = msec;
	
	uartWritePSTR("dram write speed: 4 KiB / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
	
	uint8_t arr[1024];
	
	before = msec;
	dramReadFPM(0x000000, sizeof(arr), arr);
	dramReadFPM(0x000000, sizeof(arr), arr);
	dramReadFPM(0x000000, sizeof(arr), arr);
	dramReadFPM(0x000000, sizeof(arr), arr);
	after = msec;
	
	uartWritePSTR("dram fpm read speed: (x4) 4 KiB / ");
	uartWriteDec16(after - before);
	uartWritePSTR(" ms\n");
	
	before = msec;
	dramWriteFPM(0x000000, sizeof(arr), arr);
	dramWriteFPM(0x000000, sizeof(arr), arr);
	dramWriteFPM(0x000000, sizeof(arr), arr);
	dramWriteFPM(0x000000, sizeof(arr), arr);
	after = msec;
	
	uartWritePSTR("dram fpm write speed: (x4) 4 KiB / ");
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
	
	testSRAM();
	benchmarkSRAM();
	benchmarkDRAM();
	testRefresh();
	testDRAM();
	
	/*uint8_t arr[] = { 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55,
		0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55,
		0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55
	};
	
	dramWriteFPM(0x00446688, sizeof(arr), arr);
	dramReadFPM(0x00446688, sizeof(arr), arr);*/
	
	uartWritePSTR("------ Tests Complete ------\n");
}
