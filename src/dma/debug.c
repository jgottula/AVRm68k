#include "debug.h"
#include <util/delay.h>
#include "io.h"

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
