#include "dram.h"
#include <util/delay.h>
#include "intr.h"
#include "io.h"
#include "uart.h"

/* TODO: refresh! */
/* TODO: fast page mode! */

static void dramLoadAddrL(uint32_t addr)
{
	writeIO(&PORT_ADDRL, ADDRL_ALL, (uint8_t)addr);
	writeIO(&PORT_ADDRH, ADDRH_ALL, (uint8_t)(addr >> 2));
}

static void dramLoadAddrH(uint32_t addr)
{
	writeIO(&PORT_ADDRL, ADDRL_ALL, (uint8_t)(addr >> 10));
	writeIO(&PORT_ADDRH, ADDRH_ALL, (uint8_t)(addr >> 12));
}

uint8_t dramRead(uint32_t addr)
{
	uartWritePSTR("dramRead: guessing DRAM timings!\n");
	
	/* set the data bus to input with pull-up */
	writeIO(&DDR_DATA, DATA_ALL, 0);
	writeIO(&PORT_DATA, DATA_ALL, DATA_ALL);
	
	/* put the high part of the address as the row number */
	dramLoadAddrH(addr);
	
	/* assert RAS and wait for it to load */
	writeIO(&PORT_DRAM, DRAM_RAS, 0);
	_delay_us(1);
	
	/* put the low part of the address as the column number */
	dramLoadAddrL(addr);
	
	/* assert CAS and wait for it to load */
	writeIO(&PORT_DRAM, DRAM_CAS, 0);
	_delay_us(1);
	
	/* read from the data bus */
	uint8_t byte = readIO(&PORT_DATA, DATA_ALL);
	
	/* reset RAS and CAS */
	writeIO(&PORT_DRAM, DRAM_RAS | DRAM_CAS, DRAM_RAS | DRAM_CAS);
	
	return byte;
}

void dramWrite(uint32_t addr, uint8_t byte)
{
	uartWritePSTR("dramWrite: guessing DRAM timings!\n");
	/* remember to unset WE when done */
}

void dramRefresh(void)
{
	uartWritePSTR("dramRefresh: guessing DRAM timings!\n");
	uartWritePSTR("dramRefresh: start\n");
	
	uint16_t start = msec;
	
	for (uint8_t i = 0b00; i <= 0b11; ++i)
	{
		uint8_t j = 0x00;
		
		/* load the high part of the address */
		writeIO(&PORT_ADDRH, ADDRH_ALL, i << 6);
		
		do
		{
			/* load the low part of the address */
			writeIO(&PORT_ADDRL, ADDRL_ALL, j);
			
			/* assert RAS, wait, and then reset RAS */
			writeIO(&PORT_DRAM, DRAM_RAS, 0);
			_delay_us(1);
			writeIO(&PORT_DRAM, DRAM_RAS, DRAM_RAS);
			////// put another delay here??
		}
		while (++j != 0x00);
	}
	
	uint16_t finish = msec;
	
	uartWritePSTR("dramRefresh: finish\n");
	
	uint16_t time = finish - start;
	uartWritePSTR("dramRefresh: took ");
	uartWriteDec16(time);
	uartWritePSTR(" ms\n");
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
