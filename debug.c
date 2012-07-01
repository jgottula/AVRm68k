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

void dbgHeader(void)
{
	uartWritePSTR("[AVRm68k] ");
}
