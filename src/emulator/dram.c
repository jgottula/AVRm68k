/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"


#if DRAM_SAFE_MODE
#define delay() _NOP(); _NOP()
#else
#define delay() 
#endif

#define delayAlways() _NOP(); _NOP()


uint16_t lastAddr;


static void dramLoadAddrBus(uint16_t newAddr)
{
	if (newAddr == lastAddr)
		return;
	
	uint8_t addr = (PORT_DRAM_ADDR & ~DRAM_ADDR);
	
	/* if only the low 4 bits have changed, we don't need to send all 12 bits
	 * through the flip-flop stages; because of the shared clock (octal
	 * flip-flop rather than dual quad), changes in the middle 4 bits require
	 * that the high 4 bits be updated as well */
	if ((lastAddr & 0xff0) != (newAddr & 0xff0))
	{
		uint8_t ctrl = PORT_DRAM_CTRL;
		
		/* load the high 4 bits first, for 2 flip-flop stages */
		PORT_DRAM_ADDR = (addr | ((newAddr >> 8) & DRAM_ADDR));
		PORT_DRAM_CTRL = ctrl ^ DRAM_CTRL_FFCLK;
		PORT_DRAM_CTRL = ctrl;
		
		/* load the middle 4 bits next, for a single flip-flop stage */
		PORT_DRAM_ADDR = (addr | ((newAddr >> 4) & DRAM_ADDR));
		PORT_DRAM_CTRL = ctrl ^ DRAM_CTRL_FFCLK;
		PORT_DRAM_CTRL = ctrl;
	}
	
	/* load the low 4 bits; no flip-flop activity required */
	PORT_DRAM_ADDR = (addr | (newAddr & DRAM_ADDR));
	
	lastAddr = newAddr;
}

/* WARNING: because the DRAM refresh procedure is done within an ISR and could
 * happen at any time, these functions MUST use ATOMIC_BLOCKs */

uint8_t dramRead(uint32_t addr)
{
	bool simm0;
	uint8_t byte;
	
	/* this function uses the basic read technique (not fast page mode) */
	
	/* determine which SIMM to use based on the extra address bit */
	simm0 = ((addr & (1UL << 24)) == 0);
	
	/* set the data bus to input mode with pull-ups */
	DDR_DRAM_DATA = 0;
	PORT_DRAM_DATA = DRAM_DATA;
	
	delay();
	
	/* put the row number on the address bus */
	dramLoadAddrBus(addr >> 12);
	
	delay();
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* bring RAS low to load the row */
		if (simm0)
			PORT_DRAM_STROBE &= ~DRAM_STROBE_RAS0;
		else
			PORT_DRAM_STROBE &= ~DRAM_STROBE_RAS1;
		
		delay();
		
		delay();
		
		/* put the column number on the address bus */
		dramLoadAddrBus(addr);
		
		delay();
		
		/* bring CAS low to load the column (15 ns) */
		if (simm0)
			PORT_DRAM_STROBE &= ~DRAM_STROBE_CAS0;
		else
			PORT_DRAM_STROBE &= ~DRAM_STROBE_CAS1;
		
		delayAlways();
		
		/* read from the data bus */
		byte = PIN_DRAM_DATA;
		
		delay();
		
		/* reset all strobe lines */
		writeIO(&PORT_DRAM_STROBE, DRAM_STROBE_ALL, DRAM_STROBE_ALL);
	}
	
	return byte;
}

void dramWrite(uint32_t addr, uint8_t byte)
{
	bool simm0;
	
	/* this function uses the basic write technique (not fast page mode) */
	
	/* determine which SIMM to use based on the extra address bit */
	simm0 = ((addr & (1UL << 24)) == 0);
	
	/* set the data bus to output mode */
	DDR_DRAM_DATA = DRAM_DATA;
	
	delay();
	
	/* put the row number on the address bus */
	dramLoadAddrBus(addr >> 12);
	
	delay();
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* bring RAS low to load the row */
		if (simm0)
			PORT_DRAM_STROBE &= ~DRAM_STROBE_RAS0;
		else
			PORT_DRAM_STROBE &= ~DRAM_STROBE_RAS1;
		
		delay();
		
		/* assert the write enable bit */
		PORT_DRAM_CTRL &= ~DRAM_CTRL_WRITE;
		
		delay();
		
		/* put the byte on the data bus */
		PORT_DRAM_DATA = byte;
		
		delay();
		
		/* put the column number on the address bus */
		dramLoadAddrBus(addr);
		
		delay();
		
		/* bring CAS low to load the column (15 ns) */
		if (simm0)
			PORT_DRAM_STROBE &= ~DRAM_STROBE_CAS0;
		else
			PORT_DRAM_STROBE &= ~DRAM_STROBE_CAS1;
		
		delayAlways();
		
		/* clear the write enable bit */
		PORT_DRAM_CTRL |= DRAM_CTRL_WRITE;
		
		/* reset all strobe lines */
		writeIO(&PORT_DRAM_STROBE, DRAM_STROBE_ALL, DRAM_STROBE_ALL);
	}
}

void dramReadFPM(uint32_t addr, uint16_t len, uint8_t *dest)
{
	bool simm0;
	
	/* this function uses fast page mode */
	
	/* determine which SIMM to use based on the extra address bit */
	simm0 = ((addr & (1UL << 24)) == 0);
	
	/* rolling over within the page is unexpected behavior */
	assert(addr + len <= (((addr / DRAM_COLS) + 1) * DRAM_COLS));
	
	/* set the data bus to input with pull-ups */
	DDR_DRAM_DATA = 0;
	PORT_DRAM_DATA = DRAM_DATA;
	
	delay();
	
	/* put the row number on the address bus */
	dramLoadAddrBus(addr >> 12);
	
	delay();
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* bring RAS low to load the row */
		if (simm0)
			PORT_DRAM_STROBE &= ~DRAM_STROBE_RAS0;
		else
			PORT_DRAM_STROBE &= ~DRAM_STROBE_RAS1;
		
		delay();
		
		delay();
		
		/* put the column number on the address bus */
		dramLoadAddrBus(addr);
		
		delay();
		
		do
		{
			/* bring CAS low to load the column (15 ns) */
			if (simm0)
				PORT_DRAM_STROBE &= ~DRAM_STROBE_CAS0;
			else
				PORT_DRAM_STROBE &= ~DRAM_STROBE_CAS1;
			
			delayAlways();
			
			/* read from the data bus */
			*(dest++) = PIN_DRAM_DATA;
			
			delay();
			
			/* bring CAS high again (no need to discriminate between SIMMs) */
			PORT_DRAM_STROBE |= (DRAM_STROBE_CAS0 | DRAM_STROBE_CAS1);
			
			delay();
			
			/* increment the column number */
			dramLoadAddrBus(++addr);
			
			delay();
		}
		while (--len != 0);
		
		/* reset all strobe lines */
		writeIO(&PORT_DRAM_STROBE, DRAM_STROBE_ALL, DRAM_STROBE_ALL);
	}
}

void dramWriteFPM(uint32_t addr, uint16_t len, const uint8_t *src)
{
	bool simm0;
	
	/* this function uses fast page mode */
	
	/* determine which SIMM to use based on the extra address bit */
	simm0 = ((addr & (1UL << 24)) == 0);
	
	/* rolling over within the page is unexpected behavior */
	assert(addr + len <= (((addr / DRAM_COLS) + 1) * DRAM_COLS));
	
	/* set the data bus to output */
	DDR_DRAM_DATA = DRAM_DATA;
	
	delay();
	
	/* put the row number on the address bus */
	dramLoadAddrBus(addr >> 12);
	
	delay();
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		/* bring RAS low to load the row */
		if (simm0)
			PORT_DRAM_STROBE &= ~DRAM_STROBE_RAS0;
		else
			PORT_DRAM_STROBE &= ~DRAM_STROBE_RAS1;
		
		delay();
		
		/* assert the write enable bit */
		PORT_DRAM_CTRL &= ~DRAM_CTRL_WRITE;
		
		delay();
		
		/* put the column number on the address bus */
		dramLoadAddrBus(addr);
		
		delay();
		
		do
		{
			/* put the data on the data bus */
			PORT_DRAM_DATA = *(src++);
			
			delay();
			
			/* bring CAS low to load the column (15 ns) */
			if (simm0)
				PORT_DRAM_STROBE &= ~DRAM_STROBE_CAS0;
			else
				PORT_DRAM_STROBE &= ~DRAM_STROBE_CAS1;
			
			delayAlways();
			
			/* bring CAS high again (no need to discriminate between SIMMs) */
			PORT_DRAM_STROBE |= (DRAM_STROBE_CAS0 | DRAM_STROBE_CAS1);
			
			delay();
			
			/* increment the column number */
			dramLoadAddrBus(++addr);
			
			delay();
		}
		while (--len != 0);
		
		/* clear the write enable bit */
		PORT_DRAM_CTRL |= DRAM_CTRL_WRITE;
		
		/* reset all strobe lines */
		writeIO(&PORT_DRAM_STROBE, DRAM_STROBE_ALL, DRAM_STROBE_ALL);
	}
}

void dramRefresh(void)
{
	/* this function doesn't need to save/restore state because the non-ISR dram
	 * functions are wrapped in ATOMIC_BLOCKs; DO NOT modify DDR_DRAM_DATA or
	 * PORT_DRAM_DATA within this function */
	
	/* this function uses the cas-before-ras refresh method */
	
	/* we assume that the write-enable line will be clear */
	
	/* bring both CAS strobes low to start the refresh sequence */
	PORT_DRAM_STROBE &= ~(DRAM_STROBE_CAS0 | DRAM_STROBE_CAS0);
	
	for (uint16_t i = 0; i < DRAM_ROWS; ++i)
	{
		/* wait for RAS precharge (40 ns = 1 cycle @ 20 MHz) */
		delay();
		
		/* bring both RAS strobes low to refresh the next row */
		PORT_DRAM_STROBE &= ~(DRAM_STROBE_RAS0 | DRAM_STROBE_RAS1);
		
		/* wait for the RAS pulse time (60 ns = 2 cycles @ 20 MHz) */
		delayAlways();
		delay();
		
		/* bring RAS high again */
		PORT_DRAM_STROBE |= (DRAM_STROBE_RAS0 | DRAM_STROBE_RAS1);
	}
	
	/* reset all strobe lines */
	writeIO(&PORT_DRAM_STROBE, DRAM_STROBE_ALL, DRAM_STROBE_ALL);
}

void dramInit(void)
{
	/* set the address bus, strobe and control lines to output mode */
	_Static_assert(&DDR_DRAM_ADDR == &DDR_DRAM_STROBE,
		"this function assumes that the address lines and "
		"strobes are on the same port!");
	writeIO(&DDR_DRAM_ADDR, DRAM_ADDR | DRAM_STROBE_ALL,
		DRAM_ADDR | DRAM_STROBE_ALL);
	writeIO(&DDR_DRAM_CTRL, DRAM_CTRL_ALL, DRAM_CTRL_ALL);
	
	/* set both /ras, both /cas, and /we high to begin with */
	writeIO(&PORT_DRAM_STROBE, DRAM_STROBE_ALL, DRAM_STROBE_ALL);
	writeIO(&PORT_DRAM_CTRL, DRAM_CTRL_ALL, DRAM_CTRL_WRITE);
	
	/* delay on init as required by the spec */
	_delay_us(200);
	
	/* precaution, in case interrupts are already enabled */
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		/* run 8 ras cycles as per spec */
		for (uint8_t i = 8; i != 0; --i)
		{
			PORT_DRAM_STROBE &= ~(DRAM_STROBE_RAS0 | DRAM_STROBE_RAS1);
			_NOP();
			PORT_DRAM_STROBE |= (DRAM_STROBE_RAS0 | DRAM_STROBE_RAS1);
		}
	}
	
	/* set the address to avoid only updating the low 4 bits on the assumption
	 * that what lastAddr initially contains is what is on the bus */
	lastAddr = 0xffff;
	dramLoadAddrBus(0);
	
	uartWritePSTR("DRAM initialized.\n");
}
