/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* shift register abstraction functions */

#ifndef AVRM68K_EMULATOR_SREG_H
#define AVRM68K_EMULATOR_SREG_H


enum {
	SREG_SS_FLASH = _BV(0),
	SREG_SS_SRAM  = _BV(1),
	SREG_SS_SD    = _BV(2),
	SREG_SS_RTC   = _BV(3),
	SREG_SS_ALL   = SREG_SS_FLASH | SREG_SS_SRAM | SREG_SS_SD | SREG_SS_RTC,
	
	SREG_ADDRH_8   = _BV(4),
	SREG_ADDRH_9   = _BV(5),
	SREG_ADDRH_10  = _BV(6),
	SREG_ADDRH_11  = _BV(7),
	SREG_ADDRH_ALL = SREG_ADDRH_8 | SREG_ADDRH_9 | SREG_ADDRH_10 | SREG_ADDRH_11
} ShiftRegBits;


extern uint8_t sregState;


void sregClear(void);
void sregSet(uint8_t newState);
void sregInit(void);


#endif
