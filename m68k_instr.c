#include "m68k_instr.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_mem.h"
#include "uart.h"

extern uint8_t *instr;

void instrNop(void)
{
	/* debug */
	dbgHeader();
	uartWritePSTR("nop\n");
	
	cpu.ureg.pc.l += 1;
	
	/* no flags */
}
