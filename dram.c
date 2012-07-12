#include "dram.h"
#include <avr/cpufunc.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include "debug.h"
#include "intr.h"
#include "io.h"
#include "uart.h"

#if DRAM_SAFE_MODE
#define delay() _NOP(); _NOP()
#else
#define delay() 
#endif

#define delayAlways() _NOP(); _NOP()

/* WARNING: because the refresh routine is called from an ISR and could
 * interrupt at any time, these functions MUST use ATOMIC_BLOCKs */

static void dramLoadAddrBus(uint16_t addr12)
{
	PORT_ADDRL = (uint8_t)addr12;
	writeIO(&PORT_ADDRH, ADDRH_ALL, (uint8_t)(addr12 >> 4));
}

uint8_t dramRead(uint32_t addr)
{
	uint8_t byte;
	
	/* this function uses the basic read technique (not fast page mode) */
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* set the data bus to input with pull-ups */
		DDR_DATA = 0;
		PORT_DATA = DATA_ALL;
		
		delay();
		
		/* put the row number on the address bus */
		dramLoadAddrBus(addr >> 12);
		
		delay();
		
		/* bring RAS low to load the row */
		writeIO(&PORT_DRAM, DRAM_RAS, 0);
		
		delay();
		
		/* put the column number on the address bus */
		dramLoadAddrBus(addr);
		
		delay();
		
		/* bring CAS low to load the column (15 ns) */
		writeIO(&PORT_DRAM, DRAM_CAS, 0);
		
		delayAlways();
		
		/* read from the data bus */
		byte = PIN_DATA;
		
		delay();
		
		/* reset all control lines */
		writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
	}
	
	return byte;
}

void dramWrite(uint32_t addr, uint8_t byte)
{
	/* this function uses the basic write technique (not fast page mode) */
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* set the data bus to output */
		DDR_DATA = DATA_ALL;
		
		delay();
		
		/* put the row number on the address bus */
		dramLoadAddrBus(addr >> 12);
		
		delay();
		
		/* bring RAS low to load the row */
		writeIO(&PORT_DRAM, DRAM_RAS, 0);
		
		delay();
		
		/* put the byte on the data bus and assert WE */
		PORT_DATA = byte;
		writeIO(&PORT_DRAM, DRAM_WE, 0);
		
		delay();
		
		/* put the column number on the address bus */
		dramLoadAddrBus(addr);
		
		delay();
		
		/* bring CAS low to load the column (15 ns) */
		writeIO(&PORT_DRAM, DRAM_CAS, 0);
		
		delayAlways();
		
		/* reset all control lines */
		writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
	}
}

void dramReadFPM(uint32_t addr, uint16_t len, uint8_t *dest)
{
	/* this function uses fast page mode */
	
	/* rolling over within the page is unexpected behavior */
	assert(addr + len < (((addr / DRAM_COLS) + 1) * DRAM_COLS));
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* set the data bus to input with pull-ups */
		DDR_DATA = 0;
		PORT_DATA = DATA_ALL;
		
		delay();
		
		/* put the row number on the address bus */
		dramLoadAddrBus(addr >> 12);
		
		delay();
		
		/* bring RAS low to load the row */
		writeIO(&PORT_DRAM, DRAM_RAS, 0);
		
		delay();
		
		/* put the column number on the address bus */
		dramLoadAddrBus(addr);
		
		delay();
		
		do
		{
			/* bring CAS low to load the column (15 ns) */
			writeIO(&PORT_DRAM, DRAM_CAS, 0);
			
			delayAlways();
			
			/* read from the data bus */
			*(dest++) = PIN_DATA;
			
			delay();
			
			/* bring CAS high again */
			writeIO(&PORT_DRAM, DRAM_CAS, DRAM_CAS);
			
			delay();
			
			/* increment the column number */
			PORT_ADDRL = ++addr;
			
			delay();
		}
		while (--len != 0);
		
		/* reset all control lines */
		writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
	}
}

void dramWriteFPM(uint32_t addr, uint16_t len, const uint8_t *src)
{
	/* this function uses fast page mode */
	
	/* rolling over within the page is unexpected behavior */
	assert(addr + len < (((addr / DRAM_COLS) + 1) * DRAM_COLS));
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* set the data bus to output */
		DDR_DATA = DATA_ALL;
		
		delay();
		
		/* put the row number on the address bus */
		dramLoadAddrBus(addr >> 12);
		
		delay();
		
		/* bring RAS low to load the row */
		writeIO(&PORT_DRAM, DRAM_RAS, 0);
		
		delay();
		
		/* assert the write enable bit */
		writeIO(&PORT_DRAM, DRAM_WE, 0);
		
		delay();
		
		/* put the column number on the address bus */
		dramLoadAddrBus(addr);
		
		delay();
		
		do
		{
			/* put the data on the data bus */
			PORT_DATA = *(src++);
			
			delay();
			
			/* bring CAS low to load the column (15 ns) */
			writeIO(&PORT_DRAM, DRAM_CAS, 0);
			
			delayAlways();
			
			/* bring CAS high again */
			writeIO(&PORT_DRAM, DRAM_CAS, DRAM_CAS);
			
			delay();
			
			/* increment the column number */
			PORT_ADDRL = ++addr;
			
			delay();
		}
		while (--len != 0);
		
		/* reset all control lines */
		writeIO(&PORT_DRAM, DRAM_ALL, DRAM_ALL);
	}
}

void dramRefresh(void)
{
	/* this function doesn't need to save/restore state because the non-ISR dram
	 * functions are wrapped in ATOMIC_BLOCKs */
	
	/* this function uses the cas-before-ras refresh method */
	
	/* bring CAS low to start the refresh sequence */
	writeIO(&PORT_DRAM, DRAM_CAS, 0);
	
	for (uint16_t i = 0; i < DRAM_ROWS; ++i)
	{
		/* wait for RAS precharge (40 ns = 1 cycle @ 20 MHz) */
		delay();
		
		/* bring RAS low to refresh the next row */
		writeIO(&PORT_DRAM, DRAM_RAS, 0);
		
		/* wait for the RAS pulse time (60 ns = 2 cycles @ 20 MHz) */
		delayAlways();
		delay();
		
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
	
	/* precaution, in case interrupts are already enabled */
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		/* run 8 ras cycles as per spec */
		for (uint8_t i = 8; i != 0; --i)
		{
			writeIO(&PORT_DRAM, DRAM_RAS, 0);
			_NOP();
			writeIO(&PORT_DRAM, DRAM_RAS, DRAM_RAS);
		}
	}
	
	uartWritePSTR("DRAM initialized.\n");
}
