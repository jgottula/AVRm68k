#ifndef JGOTTULA_DEBUG_H
#define JGOTTULA_DEBUG_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include "uart.h"

#define ASSERT_DIE
#undef  ASSERT_RESET

#undef HAVE_LED

#ifdef DEBUG
#define assert(x) \
	{ if (!(x)) { assertFail(PSTR(__FILE__), __LINE__); } }
#else
#define assert(x) (x)
#endif

noreturn void assertFail(const char *file, uint16_t line);
noreturn void reset(void);
void flashLED(uint16_t duration);
noreturn void blinkLEDForever(uint16_t halfPeriod);
void dbgHeader(void);

noreturn inline void die(void)
{
	cli();
	
	for ( ; ; );
}

#endif
