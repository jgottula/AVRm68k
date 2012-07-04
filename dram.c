#include "dram.h"
#include <util/atomic.h>
#include <util/delay.h>
#include "intr.h"
#include "io.h"
#include "uart.h"

/* WARNING: because the refresh routine is called from an ISR and could
 * interrupt at any time, these functions MUST use ATOMIC_BLOCKs */

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
	uint8_t byte;
	
	uartWritePSTR("dramRead: guessing DRAM timings!\n");
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
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
		byte = readIO(&PORT_DATA, DATA_ALL);
		
		/* reset RAS and CAS */
		writeIO(&PORT_DRAM, DRAM_RAS | DRAM_CAS, DRAM_RAS | DRAM_CAS);
	}
	
	return byte;
}

void dramWrite(uint32_t addr, uint8_t byte)
{
	uartWritePSTR("dramWrite: guessing DRAM timings!\n");
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* set the data bus to output */
		writeIO(&DDR_DATA, DATA_ALL, DATA_ALL);
		
		/* assert the write enable line */
		writeIO(&PORT_DRAM, DRAM_WE, 0);
		
		/* put the data on the data bus */
		writeIO(&PORT_DATA, DATA_ALL, byte);
		
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
		
		/* reset all control lines */
		writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
	}
}

void dramRefresh(void)
{
	/* WARNING: this function will always be called from an ISR */
	/* this function takes ~2 ms to execute */
	
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
