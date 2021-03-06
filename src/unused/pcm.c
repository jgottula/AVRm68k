/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

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

static void pcmWriteEnable(void)
{
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_WREN);
	spiEnd();
}

static void pcmWriteDisable(void)
{
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_WRDI);
	spiEnd();
}

static uint8_t pcmReadSR(void)
{
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_RDSR);
	
	uint8_t sr = spiByte(0x00);
	
	spiEnd();
	
	return sr;
}

static void pcmWriteSR(uint8_t sr)
{
	pcmWriteEnable();
	
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_WRSR);
	
	spiByte(sr);
	
	spiEnd();
}

void pcmReadRange(uint32_t addr, uint16_t num, uint8_t *dest)
{
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_FAST_READ);
	
	/* address */
	spiByte((addr & 0x00ff0000) >> 16);
	spiByte((addr & 0x0000ff00) >> 8);
	spiByte(addr & 0x000000ff);
	
	/* dummy byte */
	spiByte(0x00);
	
	do
		*(dest++) = spiByte(0x00);
	while (--num != 0);
	
	spiEnd();
}

uint8_t pcmReadByte(uint32_t addr)
{
	uint8_t byte;
	
	pcmReadRange(addr, 1, &byte);
	
	return byte;
}

void pcmWriteRange(uint32_t addr, uint16_t num, const uint8_t *src)
{
	pcmWriteEnable();
	
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_PP_BAWR); // not sure which PP variant to use
	
	/* address */
	spiByte((addr & 0x00ff0000) >> 16);
	spiByte((addr & 0x0000ff00) >> 8);
	spiByte(addr & 0x000000ff);
	
	do
		spiByte(*(src++));
	while (--num != 0);
	
	spiEnd();
}

void pcmWriteByte(uint32_t addr, uint8_t byte)
{
	pcmWriteRange(addr, 1, &byte);
}

void pcmSectorErase(uint32_t addr)
{
	pcmWriteEnable();
	
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_SEC_ERASE);
	
	spiByte((addr & 0x00ff0000) >> 16);
	spiByte((addr & 0x0000ff00) >> 8);
	spiByte(addr & 0x000000ff);
	
	spiEnd();
	
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_RDSR);
	
	/* warning: this probably needs a timeout */
	while ((spiByte(0x00) & PCM_SR_WIP) != 0);
	
	spiEnd();
}

void pcmBulkErase(void)
{
	pcmWriteEnable();
	
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_BULK_ERASE);
	
	spiEnd();
	
	spiBegin(SPI_SS_PCM, SPIMODE_PCM, SPIENDIAN_PCM, SPIDIV_PCM);
	spiByte(PCM_INSTR_RDSR);
	
	/* warning: this probably needs a timeout */
	while ((spiByte(0x00) & PCM_SR_WIP) != 0);
	
	spiEnd();
}

void pcmInit(void)
{
	/* check that the hardware ID is valid */
	assert(pcmReadID() == 0x20da18);
	
	/* set the status register */
	pcmWriteSR(0x00);
}
