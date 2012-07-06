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
#include "m68k_mem.h"
#include "sd.h"
#include "spi.h"
#include "test.h"
#include "uart.h"

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
	
	uartWritePSTR("\nStarted.\n");
	
	m68kInit();
	
	testAll();
	
	while (true)
	{
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			if (badISR)
			{
				badISR = false;
				
				uartWritePSTR("Bad ISR!\n");
			}
		}
		
		m68kNext();
	}
	
	blinkLEDForever(500);
	die();
}
