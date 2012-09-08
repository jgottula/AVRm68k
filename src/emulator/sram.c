#include "all.h"

static uint8_t currentSR = SRAM_SR_MODE_BYTE;

static void setMode(uint8_t mode)
{
#warning sram codebase contains references to shift reg code
#if 0
	/* this function should only be called after an instruction sequence has
	 * been started; it will reopen an instruction sequence when finished */
	
	if (currentSR != mode)
	{
		/* write to the status register */
		spiByte(SRAM_INSTR_WRSR);
		spiByte(mode);
		
		currentSR = mode;
		
		/* start another instruction sequence */
		spiEnd();
		spiBeginQuick(SREG_SS_SRAM);
	}
#endif
}

uint8_t sramReadByte(uint16_t addr)
{
#warning sram codebase contains references to shift reg code
#if 0
	uint8_t byte;
	
	/* start an instruction sequence */
	spiBegin(SREG_SS_SRAM, SPIMODE_SRAM, SPIENDIAN_SRAM, SPIDIV_SRAM);
	
	/* set to byte mode (if necessary) */
	setMode(SRAM_SR_MODE_BYTE);
	
	/* issue the read instruction */
	spiByte(SRAM_INSTR_READ);
	
	/* write the address */
	spiByte(addr >> 8);
	spiByte(addr);
	
	/* read the data */
	byte = spiByte(0xff);
	
	spiEnd();
	
	return byte;
#endif
}

void sramWriteByte(uint16_t addr, uint8_t byte)
{
#warning sram codebase contains references to shift reg code
#if 0
	/* start an instruction sequence */
	spiBegin(SREG_SS_SRAM, SPIMODE_SRAM, SPIENDIAN_SRAM, SPIDIV_SRAM);
	
	/* set to byte mode (if necessary) */
	setMode(SRAM_SR_MODE_BYTE);
	
	/* issue the write instruction */
	spiByte(SRAM_INSTR_WRITE);
	
	/* write the address */
	spiByte(addr >> 8);
	spiByte(addr);
	
	/* write the data */
	spiByte(byte);
	
	spiEnd();
#endif
}

void sramReadSeq(uint16_t addr, uint16_t len, uint8_t *dest)
{
#warning sram codebase contains references to shift reg code
#if 0
	/* start an instruction sequence */
	spiBegin(SREG_SS_SRAM, SPIMODE_SRAM, SPIENDIAN_SRAM, SPIDIV_SRAM);
	
	/* set to sequential mode (if necessary) */
	setMode(SRAM_SR_MODE_SEQ);
	
	/* issue the read instruction */
	spiByte(SRAM_INSTR_READ);
	
	/* write the address */
	spiByte(addr >> 8);
	spiByte(addr);
	
	/* read the data */
	do
		*(dest++) = spiByte(0x00);
	while (--len != 0);
	
	spiEnd();
#endif
}

void sramWriteSeq(uint16_t addr, uint16_t len, const uint8_t *src)
{
#warning sram codebase contains references to shift reg code
#if 0
	/* start an instruction sequence */
	spiBegin(SREG_SS_SRAM, SPIMODE_SRAM, SPIENDIAN_SRAM, SPIDIV_SRAM);
	
	/* set to sequential mode (if necessary) */
	setMode(SRAM_SR_MODE_SEQ);
	
	/* issue the write instruction */
	spiByte(SRAM_INSTR_WRITE);
	
	/* write the address */
	spiByte(addr >> 8);
	spiByte(addr);
	
	/* write the data */
	do
		spiByte(*(src++));
	while (--len != 0);
	
	spiEnd();
#endif
}
