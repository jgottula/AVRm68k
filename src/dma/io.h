#ifndef JGOTTULA_DMA_IO_H
#define JGOTTULA_DMA_IO_H

#include "../common/io.h"

#define DDR_DATA  DDRA
#define PORT_DATA PORTA
#define PIN_DATA  PINA

enum PortA {
	/* PA0-PA7: data bus 0:7 */
};

#define DDR_  DDRB
#define PORT_ PORTB
#define PIN_  PINB

enum PortB {
	/* PB0-PB7 free */
};

#define DDR_ADDR  DDRC
#define PORT_ADDR PORTC
#define PIN_ADDR  PINC

enum PortC {
	/* PC0-PC7: addr bus 0:7 */
	/* PC0-PC3: addr bus 8:11 (latched) */
};

#define DDR_  DDRD
#define PORT_ PORTD
#define PIN_  PIND

enum PortD {
	/* PD0-PD7 free */
};

#endif
