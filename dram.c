#include "dram.h"
#include <avr/cpufunc.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include "debug.h"
#include "intr.h"
#include "io.h"
#include "uart.h"

/* WARNING: because the refresh routine is called from an ISR and could
 * interrupt at any time, these functions MUST use ATOMIC_BLOCKs */

static void dramLoadAddrBus(uint16_t addr12)
{
	writeIO(&PORT_ADDRL, ADDRL_ALL, (uint8_t)addr12);
	writeIO(&PORT_ADDRH, ADDRH_ALL, (uint8_t)(addr12 >> 4));
}

#warning dramRead: REWRITE!
uint8_t dramRead(uint32_t addr)
{
	uint8_t byte;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* set the data bus to input with no pull-up */
		writeIO(&DDR_DATA, DATA_ALL, 0);
		writeIO(&PORT_DATA, DATA_ALL, 0);
		
		/* put the high part of the address as the row number */
		dramLoadAddrBus(addr >> 12);
		
		/* assert RAS and wait for it to load */
		writeIO(&PORT_DRAM, DRAM_RAS, 0);
		_delay_us(1);
		
		/* put the low part of the address as the column number */
		dramLoadAddrBus(addr);
		
		/* assert CAS and wait for it to load */
		writeIO(&PORT_DRAM, DRAM_CAS, 0);
		_delay_us(1);
		
		/* read from the data bus */
		byte = readIO(&PIN_DATA, DATA_ALL);
		
		/* reset all control lines */
		writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
	}
	
	return byte;
}

#warning dramWrite: REWRITE!
void dramWrite(uint32_t addr, uint8_t byte)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* set the data bus to output */
		writeIO(&DDR_DATA, DATA_ALL, DATA_ALL);
		
		/* put the data on the data bus */
		writeIO(&PORT_DATA, DATA_ALL, byte);
		
		/* put the high part of the address as the row number */
		dramLoadAddrBus(addr >> 12);
		
		/* assert RAS and wait for it to load */
		writeIO(&PORT_DRAM, DRAM_RAS, 0);
		_delay_us(1);
		
		/* assert the write enable line */
		writeIO(&PORT_DRAM, DRAM_WE, 0);
		
		/* put the low part of the address as the column number */
		dramLoadAddrBus(addr);
		
		/* assert CAS and wait for it to load */
		writeIO(&PORT_DRAM, DRAM_CAS, 0);
		_delay_us(1);
		
		/* reset all control lines */
		writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
	}
}

void dramRefresh(void)
{
	/* this function doesn't need to save/restore state because the non-ISR dram
	 * functions are wrapped in ATOMIC_BLOCKs */
	
	/* this function uses the cas-before-ras refresh method */
	
	writeIO(&PORT_DRAM, DRAM_CAS, 0);
	
	for (uint32_t i = 0x0000; i < DRAM_SIZE / (1 << 12); ++i)
	{
		/* wait for RAS precharge (100 ns = 2 cycles @ 20 MHz) */
		_NOP();
		_NOP();
		
		/* bring RAS low to refresh the next row */
		writeIO(&PORT_DRAM, DRAM_RAS, 0);
		
		/* wait for the RAS pulse time (60 ns = 2 cycles @ 20 MHz) */
		_NOP();
		
		/* bring RAS high again */
		writeIO(&PORT_DRAM, DRAM_RAS, DRAM_RAS);
	}
	
	/* reset all control lines */
	writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
}

void dramInit(void)
{
	/* set the address bus and control lines to output mode */
	writeIO(&DDR_ADDRH, ADDRH_ALL, ADDRH_ALL);
	writeIO(&DDR_ADDRL, ADDRL_ALL, ADDRL_ALL);
	writeIO(&DDR_DRAM, DRAM_ALL, DRAM_ALL);
	
	/* deactivate all control lines */
	writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
	
	/* delay on init as required by the spec */
	_delay_us(200);
	
	/* run 8 refresh cycles (spec) */
	for (uint8_t i = 8; i != 0; --i)
	{
		_delay_ms(DRAM_REFRESH_FREQ);
		dramRefresh();
	}
	
	uartWritePSTR("DRAM initialized.\n");
}
