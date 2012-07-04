#ifndef JGOTTULA_IO_H
#define JGOTTULA_IO_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

typedef volatile uint8_t *reg_t;
typedef volatile uint16_t *reg16_t;


/*enum PortA {
};*/


#define DDR_SD  DDRB
#define PORT_SD PORTB
#define PIN_SD  PINB

#define DDR_SPI  DDRB
#define PORT_SPI PORTB
#define PIN_SPI  PINB

enum PortB {
	SD_DETECT     = _BV(PB2),
	
	SPI_SS_SD     = _BV(PB3),
	SPI_SS_NULL   = _BV(PB4),
	SPI_SS_ALL    = SPI_SS_NULL | SPI_SS_SD,
	
	SPI_MOSI      = _BV(PB5),
	SPI_MISO      = _BV(PB6),
	SPI_SCK       = _BV(PB7),
	SPI_ALL       = SPI_MOSI | SPI_MISO | SPI_SCK
};


/*enum PortC {
};*/


#define DDR_UART  DDRD
#define PORT_UART PORTD
#define PIN_UART  PIND

enum PortD {
	UART_RX      = _BV(PD0),
	UART_TX      = _BV(PD1),
	UART_CTS     = _BV(PD2),
	UART_RTS     = _BV(PD3)
};


/* WARNING: these write functions may need to be wrapped in ATOMIC_BLOCK()
 * because of their read/modify/write nature and the possibility that an ISR
 * could be RMW'ing the same register */

inline uint8_t readIO(reg_t reg, uint8_t mask)
{
	return *reg & mask;
}

inline void writeIO(reg_t reg, uint8_t mask, uint8_t val)
{
	*reg = readIO(reg, ~mask) | (val & mask);
}

inline uint16_t readIO16(reg16_t reg, uint16_t mask)
{
	return *reg & mask;
}

inline void writeIO16(reg16_t reg, uint16_t mask, uint16_t val)
{
	*reg = readIO16(reg, ~mask) | (val & mask);
}

#endif
