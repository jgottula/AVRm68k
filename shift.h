#ifndef JGOTTULA_SHIFT_H
#define JGOTTULA_SHIFT_H

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

enum {
	SHIFT_SS_FLASH     = _BV(0),
	SHIFT_SS_SRAM      = _BV(1),
	SHIFT_SS_SD        = _BV(2),
	SHIFT_SS_ALL       = SHIFT_SS_FLASH | SHIFT_SS_SRAM | SHIFT_SS_SD,
	
	SHIFT_ADDRH_8      = _BV(3),
	SHIFT_ADDRH_9      = _BV(4),
	SHIFT_ADDRH_10     = _BV(5),
	SHIFT_ADDRH_11     = _BV(6),
	SHIFT_ADDRH_ALL    = SHIFT_ADDRH_8 | SHIFT_ADDRH_9 |
	                     SHIFT_ADDRH_10 | SHIFT_ADDRH_11
	
	/* bit 7 is currently unused */
} ShiftBits;

extern uint8_t shift;

void shiftClear(void);
void shiftSet(uint8_t new);
void shiftInit(void);

#endif
