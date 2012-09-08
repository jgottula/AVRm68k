#include "all.h"

noreturn void main(void)
{
	/* prevent reset loops after a soft reset */
	writeIO(&MCUSR, _BV(WDRF), 0);
	wdt_disable();
	
	uartInit();
	uartWritePSTR("\nStarted.\n");
	
	//sregInit();
	dramInit();
	intrInit();
	spiInit();
	//sdInit();
	m68kInit();
	
	uartWritePSTR("Init complete.\n");
	
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
