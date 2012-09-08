/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* interrupt handlers */

#ifndef AVRM68K_EMULATOR_INTR_H
#define AVRM68K_EMULATOR_INTR_H


/* msec counts centiseconds
 * sec counts seconds */
extern volatile uint16_t msec, sec;
extern volatile bool badISR;
extern volatile bool enableDRAMRefresh;


void intrInit(void);


#endif
