#include "shift.h"
#include "io.h"
#include "uart.h"

uint8_t shift;

static void shiftLatch()
{
	PORT_SHIFT |= SHIFT_LATCH;
	PORT_SHIFT &= ~SHIFT_LATCH;
}

static void shiftOut()
{
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (shift & _BV(i))
			PORT_SHIFT |= SHIFT_DATA;
		else
			PORT_SHIFT &= ~SHIFT_DATA;
		
		PORT_SHIFT |= SHIFT_CLK;
		PORT_SHIFT &= ~SHIFT_CLK;
	}
}

void shiftClear(void)
{
	PORT_SHIFT &= ~SHIFT_CLEAR;
	PORT_SHIFT |= SHIFT_CLEAR;
}

void shiftSet(uint8_t new)
{
	/* only shift if the value has changed */
	if (shift != new)
	{
		shift = new;
		
		if (new != 0)
		{
			shiftOut();
			shiftLatch();
		}
		else
			shiftClear();
	}
}

void shiftInit(void)
{
	/* set all shift pins to output, and both clocks low */
	writeIO(&DDR_SHIFT, SHIFT_ALL, SHIFT_ALL);
	writeIO(&PORT_SHIFT, SHIFT_ALL, 0);
	
	/* clear out the register's former state */
	shiftClear();
	
	/* ensure a consistent initial state */
	shift = 0xff;
	shiftOut();
	shiftLatch();
}
