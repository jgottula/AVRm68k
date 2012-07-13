#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>
#include "debug.h"
#include "io.h"
#include "shift.h"

static void spiSelect(uint8_t slave)
{
	/* ensure valid input */
	assert((slave & ~SHIFT_SS_ALL) == 0);
	
	/* set only the given slave select pin low */
	shiftSet((shift | SHIFT_SS_ALL) & ~slave);
}

static void spiUnselect(void)
{
	/* set all slave select pins high */
	shiftSet(shift | SHIFT_SS_ALL);
}

static void spiSetMode(uint8_t mode, uint8_t endian, uint8_t divider)
{
	uint8_t cReg = 0;
	
	/* set the mode */
	if (mode & SPIMODE_CPOL)
		cReg |= _BV(CPOL);
	if (mode & SPIMODE_CPHA)
		cReg |= _BV(CPHA);
	
	/* set the endianness */
	if (endian == SPIENDIAN_LITTLE)
		cReg |= _BV(DORD);
	
	/* set the divider */
	if (divider & SPIDIV_SPR0)
		cReg |= _BV(SPR0);
	if (divider & SPIDIV_SPR1)
		cReg |= _BV(SPR1);
	
	/* write out to the registers */
	writeIO(&SPCR, _BV(CPOL) | _BV(CPHA) | _BV(DORD) | _BV(SPR1) | _BV(SPR0),
		cReg);
	writeIO(&SPSR, _BV(SPI2X), (divider & SPIDIV_SPI2X) ? _BV(SPI2X) : 0);
}

void spiBegin(uint8_t slave, uint8_t mode, uint8_t endian, uint8_t divider)
{
	spiSetMode(mode, endian, divider);
	spiSelect(slave);
}

void spiBeginQuick(uint8_t slave)
{
	spiSelect(slave);
}

uint8_t spiByte(uint8_t byte)
{
	/* write the byte into the SPI data register */
	SPDR = byte;
	
	/* wait for the byte to get sent over the bus */
	loop_until_bit_is_set(SPSR, SPIF);
	
	/* get the slave's response */
	return SPDR;
}

void spiEnd(void)
{
	spiUnselect();
}

void spiInit(void)
{
	/* set SS, SCLK and MOSI to output mode, and MISO to input mode */
	writeIO(&DDR_SPI, SPI_ALL, SPI_ALL ^ SPI_MISO);
	writeIO(&PORT_SPI, SPI_SS_NULL, SPI_SS_NULL);
	
	/* ensure that all slave select pins are high */
	spiUnselect();
	
	/* disable power saving for SPI, and enable hardware SPI with interrupts
	 * disabled and master mode enabled */
	writeIO(&PRR0, _BV(PRSPI), 0);
	writeIO(&SPCR, _BV(SPE) | _BV(MSTR), _BV(SPE) | _BV(MSTR));
	
	/* read the status and data registers to clear the state */
	readIO(&SPSR, 0xff);
	readIO(&SPDR, 0xff);
}
