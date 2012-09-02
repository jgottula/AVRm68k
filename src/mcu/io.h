#ifndef JGOTTULA_MCU_IO_H
#define JGOTTULA_MCU_IO_H

#include "../common/io.h"

#define DDR_DRAM_ADDR   DDRA
#define PORT_DRAM_ADDR  PORTA
#define PIN_DRAM_ADDR   PINA

#define DDR_DRAM_STROBE  DDRA
#define PORT_DRAM_STROBE PORTA
#define PIN_DRAM_STROBE  PINA

enum PortA {
	/* PA[0:7] => addr[4:11] (latched) */
	DRAM_ADDR_HIGH_LATCHED = 0xff,
	
	/* PA[0:3] => addr[0:3] (direct) */
	DRAM_ADDR_LOW          = 0x0f,
	
	/* PA[4:7] => {ras0 cas0 ras1 cas1} (direct) */
	DRAM_STROBE_RAS0       = _BV(PA4),
	DRAM_STROBE_CAS0       = _BV(PA5),
	DRAM_STROBE_RAS1       = _BV(PA6),
	DRAM_STROBE_CAS1       = _BV(PA7),
	DRAM_STROBE_ALL        = DRAM_STROBE_RAS0 | DRAM_STROBE_CAS0 |
	                         DRAM_STROBE_RAS1 | DRAM_STROBE_CAS1
};

#define DDR_SPI   DDRB
#define PORT_SPI  PORTB
#define PIN_SPI   PINB

enum PortB {
	/* PB0 free */
	
	/* PB1 reserved for CLKOUT */
	
	/* PB2-PB3 free */
	
	SPI_SS_NULL = _BV(PB4),
	SPI_MOSI    = _BV(PB5),
	SPI_MISO    = _BV(PB6),
	SPI_SCK     = _BV(PB7),
	SPI_ALL     = SPI_SS_NULL | SPI_MOSI | SPI_MISO | SPI_SCK
};

#define DDR_DRAM_DATA  DDRC
#define PORT_DRAM_DATA PORTC
#define PIN_DRAM_DATA  PINC

enum PortC {
	/* PC[0:7] <=> data[0:7] */
	DRAM_DATA = 0xff
};

#define DDR_UART      DDRD
#define PORT_UART     PORTD
#define PIN_UART      PIND

#define DDR_DRAM_CTRL  DDRD
#define PORT_DRAM_CTRL PORTD
#define PIN_DRAM_CTRL  PIND

enum PortD {
	UART_RX         = _BV(PD0),
	UART_TX         = _BV(PD1),
	UART_ALL        = UART_RX | UART_TX,
	
	/* PD2-PD5 free */
	
	DRAM_CTRL_LATCH = _BV(PD6),
	DRAM_CTRL_WRITE = _BV(PD7),
	DRAM_CTRL_ALL   = DRAM_CTRL_LATCH | DRAM_CTRL_WRITE
};

#endif
