#include "dram.h"
#include "io.h"

/* TODO: refresh! */



#include "uart.h"
#define writeIO_DEBUG(_reg, _mask, _val) \
{ \
	uartWriteStr(#_reg ": "); \
	uartWriteHex8(_val & _mask, false); \
	uartWriteChr('\n'); \
}

uint8_t dramRead(uint32_t addr)
{
	uartWritePSTR("dramRead\n");
	
	/* set the data bus to input with pull-up */
	writeIO(&DDR_DATA, DATA_ALL, 0);
	writeIO(&PORT_DATA, DATA_ALL, DATA_ALL);
	
	/* don't assert the write enable line */
	writeIO(&PORT_DRAM, DRAM_WE, DRAM_WE);
	
	/* put the high part of the address as the row number */
	writeIO_DEBUG(&PORT_ADDRL, ADDRL_ALL, (uint8_t)(addr >> 10));
	writeIO_DEBUG(&PORT_ADDRH, ADDRH_ALL, (uint8_t)(addr >> 12));
	
	/* assert RAS */
	
	/* put the low part of the address as the column number */
	writeIO_DEBUG(&PORT_ADDRL, ADDRL_ALL, (uint8_t)addr);
	writeIO_DEBUG(&PORT_ADDRH, ADDRH_ALL, (uint8_t)(addr >> 2));
	
	return 0;
}

void dramInit(void)
{
	/* set the address bus and control lines to output mode */
	writeIO(&DDR_ADDRH, ADDRH_ALL, ADDRH_ALL);
	writeIO(&DDR_ADDRL, ADDRL_ALL, ADDRL_ALL);
	writeIO(&DDR_DRAM, DRAM_ALL, DRAM_ALL);
	
	/* deactivate all control lines */
	writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
}
