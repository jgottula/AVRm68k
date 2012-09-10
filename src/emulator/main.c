/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

noreturn void main(void)
{
	/* prevent reset loops after a soft reset */
	writeIO(&MCUSR, _BV(WDRF), 0);
	wdt_disable();
	
	uartInit();
	uartWritePSTR("\nStarted.\n");
	
	dbgInit();
	//sregInit();
	dramInit();
	intrInit();
	spiInit();
	//sdInit();
	m68kInit();
	
	uartWritePSTR("Init complete.\n");
	
	/*dbgBreak();*/
	
	uint16_t localTime = 0;
	for ( ; ; )
	{
		uartWritePSTR("tick\n");
		_delay_ms(1000);
	}
	
	/*testAll();*/
	
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
	
	die();
}
