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

void pcmInit(void)
{
	uint32_t id = pcmReadID();
	
	uartWriteHex32(id, false);
	
	/* check that the ID is correct */
	assert(id == 0x0020da18);
}
