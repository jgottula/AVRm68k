/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

void assertFail(const char *file, uint16_t line)
{
	if (uartEnabled())
	{
		uartWritePSTR("\nAssertion failed at ");
		uartWriteStr(file);
		uartWriteChr(':');
		uartWriteDec16(line, false);
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
