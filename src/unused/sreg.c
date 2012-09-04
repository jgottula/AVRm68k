#include "sreg.h"
#include "io.h"
#include "uart.h"

uint8_t sregState;

static void sregLatch()
{
	PORT_SREG |= SREG_LATCH;
	PORT_SREG &= ~SREG_LATCH;
}

static void sregOut()
{
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (sregState & _BV(i))
			PORT_SREG |= SREG_DATA;
		else
			PORT_SREG &= ~SREG_DATA;
		
		PORT_SREG |= SREG_CLK;
		PORT_SREG &= ~SREG_CLK;
	}
}

void sregClear(void)
{
	/* toggle the clear line */
	PORT_SREG &= ~SREG_CLEAR;
	PORT_SREG |= SREG_CLEAR;
}

void sregSet(uint8_t newState)
{
	/* only shift if the value has changed */
	if (sregState != newState)
	{
		sregState = newState;
		
		if (newState != 0)
		{
			sregOut();
			sregLatch();
		}
		else
			sregClear();
	}
}

void sregInit(void)
{
	/* set all shift pins to output, and both clocks low */
	writeIO(&DDR_SREG, SREG_ALL, SREG_ALL);
	writeIO(&PORT_SREG, SREG_ALL, 0);
	
	/* clear out the register's former state */
	sregClear();
	
	/* ensure a consistent initial state */
	sregState = 0xff;
	sregOut();
	sregLatch();
}
