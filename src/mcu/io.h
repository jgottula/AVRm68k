#ifndef JGOTTULA_MCU_IO_H
#define JGOTTULA_MCU_IO_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

typedef volatile uint8_t *reg_t;
typedef volatile uint16_t *reg16_t;


#if 0
#define DDR_  DDRA
#define PORT_ PORTA
#define PIN_  PINA

enum PortA {
	/* PA0-PA7 free */
};
#endif


#define DDR_SPI   DDRB
#define PORT_SPI  PORTB
#define PIN_SPI   PINB

enum PortB {
	/* PB0 free */
	
	/* PB1 reserved for CLKOUT */
	
	/* PB1-PB3 free */
	
	SPI_SS_NULL = _BV(PB4),
	SPI_MOSI    = _BV(PB5),
	SPI_MISO    = _BV(PB6),
	SPI_SCK     = _BV(PB7),
	SPI_ALL     = SPI_SS_NULL | SPI_MOSI | SPI_MISO | SPI_SCK
};


#if 0
#define DDR_  DDRC
#define PORT_ PORTC
#define PIN_  PINC

enum PortC {
	/* PC0-PC7 free */
};
#endif


#define DDR_UART  DDRD
#define PORT_UART PORTD
#define PIN_UART  PIND

#define DDR_SREG  DDRD
#define PORT_SREG PORTD
#define PIN_SREG  PIND

enum PortD {
	UART_RX    = _BV(PD0),
	UART_TX    = _BV(PD1),
	UART_ALL   = UART_RX | UART_TX,
	
	/* PD2-PD3 free */
	
	SREG_CLEAR = _BV(PD4),
	SREG_DATA  = _BV(PD5),
	SREG_CLK   = _BV(PD6),
	SREG_LATCH = _BV(PD7),
	SREG_ALL   = SREG_CLEAR | SREG_DATA | SREG_CLK | SREG_LATCH
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
	if (mask == 0xff)
		*reg = val;
	else
		*reg = readIO(reg, ~mask) | (val & mask);
}

inline uint16_t readIO16(reg16_t reg, uint16_t mask)
{
	return *reg & mask;
}

inline void writeIO16(reg16_t reg, uint16_t mask, uint16_t val)
{
	if (mask == 0xffff)
		*reg = val;
	else
		*reg = readIO16(reg, ~mask) | (val & mask);
}

#endif
