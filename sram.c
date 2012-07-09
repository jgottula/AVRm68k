#include "sram.h"
#include "io.h"
#include "spi.h"

static uint8_t currentSR = SRAM_SR_MODE_BYTE;

static void setMode(uint8_t mode)
{
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
		spiBeginQuick(SPI_SS_SRAM);
	}
}

uint8_t sramReadByte(uint16_t addr)
{
	uint8_t byte;
	
	/* start an instruction sequence */
	spiBegin(SPI_SS_SRAM, SPIMODE_SRAM, SPIENDIAN_SRAM, SPIDIV_SRAM);
	
	/* set to byte mode (if necessary) */
	setMode(SRAM_SR_MODE_BYTE);
	
	/* issue the read instruction */
	spiByte(SRAM_INSTR_READ);
	
	/* write the address */
	spiByte(addr >> 8);
	spiByte(addr);
	
	/* read the data */
	byte = spiByte(0x00);
	
	spiEnd();
	
	return byte;
}

void sramWriteByte(uint16_t addr, uint8_t byte)
{
	/* start an instruction sequence */
	spiBegin(SPI_SS_SRAM, SPIMODE_SRAM, SPIENDIAN_SRAM, SPIDIV_SRAM);
	
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
}

void sramReadSeq(uint16_t addr, uint16_t len, uint8_t *dest)
{
	/* start an instruction sequence */
	spiBegin(SPI_SS_SRAM, SPIMODE_SRAM, SPIENDIAN_SRAM, SPIDIV_SRAM);
	
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
}

void sramWriteSeq(uint16_t addr, uint16_t len, const uint8_t *src)
{
	/* start an instruction sequence */
	spiBegin(SPI_SS_SRAM, SPIMODE_SRAM, SPIENDIAN_SRAM, SPIDIV_SRAM);
	
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
}
