#include "test.h"
#include <util/delay.h>
#include "debug.h"
#include "dram.h"
#include "intr.h"
#include "m68k_mem.h"
#include "uart.h"

static void testDRAM(void)
{
	uartWritePSTR("dram test: 1 MiB, incr pattern, 1000 ms: ");
	for (uint32_t i = 0x00000; i <= 0xfffff; ++i)
		dramWrite(i, i);
	_delay_ms(1000);
	for (uint32_t i = 0x00000; i < 0xfffff; ++i)
	{
		if (dramRead(i) != (i & 0xff))
		{
			uartWritePSTR("FAILED\n");
			die();
		}
	}
	uartWritePSTR("OK\n");
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

void testAll(void)
{
	uartWritePSTR("-------- Unit Tests --------\n");
	
	testDRAM();
	testRefresh();
	
	uartWritePSTR("----- Tests Completed ------\n");
}
