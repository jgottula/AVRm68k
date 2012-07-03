#include "pcm.h"
#include "debug.h"
#include "io.h"
#include "spi.h"

static uint32_t pcmReadID(void)
{
	uint32_t id = 0;
	
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_RDID);
	
	id |= ((uint32_t)spiByte(0xff) << 16);
	id |= ((uint32_t)spiByte(0xff) << 8);
	id |= (uint32_t)spiByte(0xff);
	
	spiEnd();
	
	return id;
}

void pcmReadRange(uint32_t addr, uint16_t num, uint8_t *dest)
{
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_FAST_READ);
	
	/* address */
	spiByte((addr & 0x00ff0000) >> 16);
	spiByte((addr & 0x0000ff00) >> 8);
	spiByte(addr & 0x000000ff);
	
	/* dummy */
	spiByte(0x00);
	
	while (num-- != 0)
		*(dest++) = spiByte(0x00);
	
	spiEnd();
}

uint8_t pcmReadByte(uint32_t addr)
{
	uint8_t byte;
	
	pcmReadRange(addr, 1, &byte);
	
	return byte;
}

void pcmInit(void)
{
	uint32_t id = pcmReadID();
	
	/* check that the ID is correct */
	assert(id == 0x0020da18);
}
