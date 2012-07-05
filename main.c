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

static void test(void)
{
	dbgHeader();
	uartWritePSTR("<<< Begin Tests >>>\n\n");
	
	for (uint32_t i = 0x00000; i < 0x00100; ++i)
		dramWrite(i, 0xff);
	
	for (uint32_t i = 0x00000; i < 0x00100; ++i)
		uartWriteHex8(dramRead(i), false);
	uartWriteChr('\n');
	
	uartWritePSTR("\nDDRA:  ");
	uartWriteHex8(DDRA, false);
	uartWritePSTR("\nPORTA: ");
	uartWriteHex8(PORTA, false);
	uartWritePSTR("\nPINA:  ");
	uartWriteHex8(PINA, false);
	
	uartWritePSTR("\nDDRB:  ");
	uartWriteHex8(DDRB, false);
	uartWritePSTR("\nPORTB: ");
	uartWriteHex8(PORTB, false);
	uartWritePSTR("\nPINB:  ");
	uartWriteHex8(PINB, false);
	
	uartWritePSTR("\nDDRC:  ");
	uartWriteHex8(DDRC, false);
	uartWritePSTR("\nPORTC: ");
	uartWriteHex8(PORTC, false);
	uartWritePSTR("\nPINC:  ");
	uartWriteHex8(PINC, false);
	
	uartWritePSTR("\nDDRD:  ");
	uartWriteHex8(DDRD, false);
	uartWritePSTR("\nPORTD: ");
	uartWriteHex8(PORTD, false);
	uartWritePSTR("\nPIND:  ");
	uartWriteHex8(PIND, false);
	
	uartWritePSTR("\n\n");
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
	
	test();
	
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
