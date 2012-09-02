#ifndef JGOTTULA_DMA_DEBUG_H
#define JGOTTULA_DMA_DEBUG_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <stdnoreturn.h>

#define ASSERT_DIE
#undef  ASSERT_RESET

#undef HAVE_LED

noreturn void reset(void);
void flashLED(uint16_t duration);
noreturn void blinkLEDForever(uint16_t halfPeriod);

noreturn inline void die(void)
{
	cli();
	
	for ( ; ; );
}

#endif
