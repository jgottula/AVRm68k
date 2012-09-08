#ifndef JGOTTULA_EMULATOR_INTR_H
#define JGOTTULA_EMULATOR_INTR_H

/* msec counts centiseconds
 * sec counts seconds */
extern volatile uint16_t msec, sec;
extern volatile bool badISR;
extern volatile bool enableDRAMRefresh;

void intrInit(void);

#endif
