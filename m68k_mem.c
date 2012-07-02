#include "m68k_mem.h"
#include "debug.h"
#include "uart.h"

/* WARNING: we may need to do endianness conversions on these */

uint32_t memRead(uint32_t addr, uint8_t size)
{
	dbgHeader();
	uartWritePSTR("memRead: reading from 0x");
	uartWriteHex32(addr, false);
	uartWritePSTR(" with size 0b");
	uartWriteBin4(size);
	uartWritePSTR("!\n");
	
	return 0xdeadbeef;
}

void memWrite(uint32_t addr, uint8_t size, uint32_t value)
{
	dbgHeader();
	uartWritePSTR("memWrite: writing 0x");
	uartWriteHex32(value, false);
	uartWritePSTR(" at 0x");
	uartWriteHex32(addr, false);
	uartWritePSTR(" with size 0b");
	uartWriteBin4(size);
	uartWritePSTR("!\n");
}
