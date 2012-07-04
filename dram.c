#include "dram.h"
#include <util/delay.h>
#include "io.h"
#include "uart.h"

/* TODO: refresh! */
/* TODO: fast page mode! */

uint8_t dramRead(uint32_t addr)
{
	uartWritePSTR("dramRead: guessing DRAM timings!\n");
	
	/* set the data bus to input with pull-up */
	writeIO(&DDR_DATA, DATA_ALL, 0);
	writeIO(&PORT_DATA, DATA_ALL, DATA_ALL);
	
	/* don't assert the write enable line */
	writeIO(&PORT_DRAM, DRAM_WE, DRAM_WE);
	
	/* put the high part of the address as the row number */
	writeIO(&PORT_ADDRL, ADDRL_ALL, (uint8_t)(addr >> 10));
	writeIO(&PORT_ADDRH, ADDRH_ALL, (uint8_t)(addr >> 12));
	
	/* assert RAS and wait for it to load */
	writeIO(&PORT_DRAM, DRAM_RAS, 0);
	_delay_us(1);
	
	/* put the low part of the address as the column number */
	writeIO(&PORT_ADDRL, ADDRL_ALL, (uint8_t)addr);
	writeIO(&PORT_ADDRH, ADDRH_ALL, (uint8_t)(addr >> 2));
	
	/* assert CAS and wait for it to load */
	writeIO(&PORT_DRAM, DRAM_CAS, 0);
	_delay_us(1);
	
	/* read from the data bus */
	uint8_t byte = readIO(&PORT_DATA, DATA_ALL);
	
	/* reset RAS and CAS */
	writeIO(&PORT_DRAM, DRAM_RAS | DRAM_CAS, DRAM_RAS | DRAM_CAS);
	
	return byte;
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
