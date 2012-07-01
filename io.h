#ifndef JGOTTULA_IO_H
#define JGOTTULA_IO_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

/* reserved IO pins: PB0 (clkout), PC6 (reset) */

typedef volatile uint8_t *reg_t;
typedef volatile uint16_t *reg16_t;

/*enum PortA {
};*/

#define SPI_DDR   DDRB
#define SPI_PORT  PORTB
#define SPI_PIN   PINB

enum PortB {
	/* PB0, PB1 are free */
	
	/* slaves should NOT use PB2 for slave select (makes SPI to switch modes) */
	SPI_SS_DEF = _BV(PB2),
	SPI_SS_ALL = 0,
	
	SPI_MOSI   = _BV(PB3),
	SPI_MISO   = _BV(PB4),
	SPI_SCK    = _BV(PB5),
	SPI_ALL    = SPI_MOSI | SPI_MISO | SPI_SCK
};

#define DRAM_DDR  DDRC
#define DRAM_PORT PORTC
#define DRAM_PIN  PINC

enum PortC {
	DRAM_RAS = _BV(PC0),
	DRAM_CAS = _BV(PC1),
	DRAM_WE  = _BV(PC2),
	
	
};

#define UART_DDR  DDRD
#define UART_PORT PORTD
#define UART_PIN  PIND

#define SHIFT_DDR  DDRD
#define SHIFT_PORT PORTD

enum PortD {
	/* PD0 and PD1 are reserved for RXD and TXD, respectively */
	
	UART_CTS     = _BV(PD2),
	UART_RTS     = _BV(PD3),
	
	SHIFT_DATA   = _BV(PD4),
	SHIFT_CLK    = _BV(PD5),
	SHIFT_LATCH  = _BV(PD6),
	SHIFT_CLEAR  = _BV(PD7),
	SHIFT_ALL    = SHIFT_DATA | SHIFT_CLK | SHIFT_LATCH | SHIFT_CLEAR
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
