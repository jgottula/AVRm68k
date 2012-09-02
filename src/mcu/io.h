#ifndef JGOTTULA_MCU_IO_H
#define JGOTTULA_MCU_IO_H

#include "../common/io.h"

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

#endif
