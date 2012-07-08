#ifndef JGOTTULA_IO_H
#define JGOTTULA_IO_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

typedef volatile uint8_t *reg_t;
typedef volatile uint16_t *reg16_t;


#define DDR_DATA  DDRA
#define PORT_DATA PORTA
#define PIN_DATA  PINA

enum PortA {
	DATA_0   = _BV(PA0),
	DATA_1   = _BV(PA1),
	DATA_2   = _BV(PA2),
	DATA_3   = _BV(PA3),
	DATA_4   = _BV(PA4),
	DATA_5   = _BV(PA5),
	DATA_6   = _BV(PA6),
	DATA_7   = _BV(PA7),
	DATA_ALL = 0xff
};


#define DDR_DRAM  DDRB
#define PORT_DRAM PORTB
#define PIN_DRAM  PINB

#define DDR_SPI   DDRB
#define PORT_SPI  PORTB
#define PIN_SPI   PINB

enum PortB {
	DRAM_CAS   = _BV(PB0),
	DRAM_WE    = _BV(PB1),
	DRAM_RAS   = _BV(PB2),
	DRAM_ALL   = DRAM_CAS | DRAM_WE | DRAM_RAS,
	
	/* PB3 is open */
	
	SPI_SS_SD  = _BV(PB4),
	SPI_SS_ALL = SPI_SS_SD,
	
	SPI_MOSI   = _BV(PB5),
	SPI_MISO   = _BV(PB6),
	SPI_SCK    = _BV(PB7),
	SPI_ALL    = SPI_MOSI | SPI_MISO | SPI_SCK
};


#define DDR_ADDRL  DDRC
#define PORT_ADDRL PORTC
#define PIN_ADDRL  PINC

enum PortC {
	ADDRL_0   = _BV(PC0),
	ADDRL_1   = _BV(PC1),
	ADDRL_2   = _BV(PC2),
	ADDRL_3   = _BV(PC3),
	ADDRL_4   = _BV(PC4),
	ADDRL_5   = _BV(PC5),
	ADDRL_6   = _BV(PC6),
	ADDRL_7   = _BV(PC7),
	ADDRL_ALL = ADDRL_0 | ADDRL_1 | ADDRL_2 | ADDRL_3 |
	            ADDRL_4 | ADDRL_5 | ADDRL_6 | ADDRL_7
};


#define DDR_UART  DDRD
#define PORT_UART PORTD
#define PIN_UART  PIND

#define DDR_ADDRH  DDRD
#define PORT_ADDRH PORTD
#define PIN_ADDRH  PIND

enum PortD {
	UART_RX   = _BV(PD0),
	UART_TX   = _BV(PD1),
	UART_ALL  = UART_RX | UART_TX,
	
	/* PD2 and PD3 are open */
	
	ADDRH_8   = _BV(PD4),
	ADDRH_9   = _BV(PD5),
	ADDRH_10  = _BV(PD6),
	ADDRH_11  = _BV(PD7),
	ADDRH_ALL = ADDRH_8 | ADDRH_9 | ADDRH_10 | ADDRH_11
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
