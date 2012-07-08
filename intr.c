#include "intr.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "dram.h"
#include "io.h"

static volatile uint8_t dramCounter = 0;
static volatile uint16_t counter = 0;
volatile uint16_t msec = 0, sec = 0;
volatile bool badISR = false;

ISR(TIMER1_COMPA_vect)
{
	uint16_t newCounter = ++counter;
	
	++msec;
	
	if (newCounter == 1000)
	{
		++sec;
		counter = 0;
	}
	
	/* refresh the dram at intervals */
	if (++dramCounter == DRAM_REFRESH_FREQ)
	{
		dramRefresh();
		dramCounter = 0;
	}
}

ISR(BADISR_vect)
{
	/* TODO: find out which vector was triggered (if possible) and relay this
	 * information to the debug output */
	badISR = true;
}

void intrInit(void)
{
	/* set TIMER1 to run at F_CPU Hz and overflow when equal to OCR1A */
	TCCR1B = _BV(CS10) | _BV(WGM12);
	
	/* set the overflow threshold to 1/1000 second */
	OCR1A = (F_CPU / 1000) - 1;
	
	/* zero out the counter */
	TCNT1 = 0;
	
	/* enable interrupt when TCNT1 equals OCR1A */
	writeIO(&TIMSK1, _BV(OCIE1A), _BV(OCIE1A));
	
	/* enable interrupts */
	sei();
}
