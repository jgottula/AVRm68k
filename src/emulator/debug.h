/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* debugging functions and macros */

#ifndef AVRM68K_EMULATOR_DEBUG_H
#define AVRM68K_EMULATOR_DEBUG_H


#define ASSERT_DIE
#undef  ASSERT_RESET


#undef HAVE_LED


#ifdef DEBUG
/* don't use PSTR() around __FILE__, as it will make duplicate copies, resulting
 * in a net size _increase_ */
#define assert(x) \
	{ if (!(x)) { assertFail(__FILE__, __LINE__); } }
#else
#define assert(x) (x)
#endif


noreturn void assertFail(const char *file, uint16_t line);
noreturn void reset(void);
void flashLED(uint16_t duration);
noreturn void blinkLEDForever(uint16_t halfPeriod);
void dumpIO();


noreturn inline void die(void)
{
	cli();
	
	for ( ; ; );
}


#endif
