#include "debug.h"
#include <util/delay.h>
#include "io.h"

void assertFail(const char *file, uint16_t line)
{
	if (uartEnabled())
	{
		dbgHeader();
		uartWritePSTR("Assertion failed at ");
		uartWritePStr(file);
		uartWriteChr(':');
		uartWriteDec16(line);
		uartWritePSTR(".\n");
	}
	
#if defined(ASSERT_DIE) && !defined(ASSERT_RESET)
	die();
#elif !defined(ASSERT_DIE) && defined(ASSERT_RESET)
	reset();
#else
#error either ASSERT_DIE or ASSERT_RESET must be defined in debug.h
#endif
}

void reset(void)
{
	cli();
	
	wdt_enable(WDTO_15MS);
	
	die();
}

void flashLED(uint16_t duration)
{
#ifdef HAVE_LED
	writeIO(&LED_DDR, LED_0, LED_0);
	writeIO(&LED_PORT, LED_0, LED_0);
#endif

	_delay_ms(duration);
	
#ifdef HAVE_LED
	writeIO(&LED_PORT, LED_0, 0);
#endif
}

void blinkLEDForever(uint16_t halfPeriod)
{
	cli();
	
#ifdef HAVE_LED
	writeIO(&LED_DDR, LED_0, LED_0);
#endif
	
	/* blink the LED forever */
	for ( ; ; )
	{
#ifdef HAVE_LED
		LED_PORT ^= LED_0;
#endif
		
		_delay_ms(halfPeriod);
	}
}

void dumpIO(void)
{
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
}

void dbgHeader(void)
{
	uartWritePSTR("[AVRm68k] ");
}
