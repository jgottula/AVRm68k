#include "dram.h"

/* avr out [8+11+3 total]
 * data bus [8]
 * addr bus [11]
 * ras, cas, we
 * 
 * avr in [8 total]
 * data bus [8]
 * 
 * ignore parity (NC)
 */
 
static void dramAssertRAS(void)
{
	/* delay in order to meet timing requirements */
}
 
static void dramResetRAS(void)
{
	
}

void dramRefresh(void)
{
	
}

void dramInit(void)
{
	/* set RAS high, etc */
}
