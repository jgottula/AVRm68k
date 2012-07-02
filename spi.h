#ifndef JGOTTULA_SPI_H
#define JGOTTULA_SPI_H

#include <stdint.h>

enum {
	SPIMODE_CPOL = 0b10,
	SPIMODE_CPHA = 0b01,
	
	SPIMODE_0    = 0b00,
	SPIMODE_1    = 0b01,
	SPIMODE_2    = 0b10,
	SPIMODE_3    = 0b11
} SPIMode;

enum {
	SPIENDIAN_BIG    = 0,
	SPIENDIAN_LITTLE = 1
} SPIEndian;

/* frequency values given assume 20 MHz operation */
enum {
	SPIDIV_SPR0  = 0b001,
	SPIDIV_SPR1  = 0b010,
	SPIDIV_SPI2X = 0b100,
	
	SPIDIV_4     = 0b000, //  5.0 MHz
	SPIDIV_16    = 0b001, // 1.25 kHz
	SPIDIV_64    = 0b010, //  312 kHz
	SPIDIV_128   = 0b011, //  156 kHz
	
	SPIDIV_2     = 0b100, // 10.0 MHz
	SPIDIV_8     = 0b101, //  5.2 MHz
	SPIDIV_32    = 0b110  //  626 kHz
} SPIDivider;

/* SPI parameters for each slave */
#define SPIMODE_MCSRAM   SPIMODE_0
#define SPIENDIAN_MCSRAM SPIENDIAN_BIG
#define SPIDIV_MCSRAM    SPIDIV_2

#define SPIMODE_SD   SPIMODE_0
#define SPIENDIAN_SD SPIENDIAN_BIG
#define SPIDIV_SD    SPIDIV_2

void spiBegin(uint8_t slave, uint8_t mode, uint8_t endian, uint8_t divider);
uint8_t spiByte(uint8_t byte);
void spiEnd(void);
void spiInit(void);

#endif
