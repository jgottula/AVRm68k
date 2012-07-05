#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "debug.h"
#include "dram.h"
#include "intr.h"
#include "io.h"
#include "m68k.h"
#include "sd.h"
#include "shift.h"
#include "spi.h"
#include "uart.h"

static void runTests(void)
{
	dbgHeader();
	uartWritePSTR("<<< Begin Tests >>>\n\n");
	
	dramTest();
	
	uartWriteChr('\n');
	dbgHeader();
	uartWritePSTR("<<< End Tests >>>\n\n");
}

noreturn void main(void)
{
	/* prevent reset loops after a soft reset */
	writeIO(&MCUSR, _BV(WDRF), 0);
	wdt_disable();
	
	uartInit();
	intrInit();
	
	dramInit();
	spiInit();
	//sdInit();
	
	uartWriteChr('\n');
	dbgHeader();
	uartWritePSTR("Started.\n");
	
	runTests();
	
	m68kInit();
	
	while (true)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if (badISR)
			{
				badISR = false;
				
				dbgHeader();
				uartWritePSTR("Bad ISR!\n");
			}
		}
		
		m68kNext();
	}
	
	blinkLEDForever(500);
	die();
}
