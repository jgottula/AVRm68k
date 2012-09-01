#ifndef JGOTTULA_INTR_H
#define JGOTTULA_INTR_H

#include <stdint.h>
#include <stdbool.h>

/* msec counts centiseconds
 * sec counts seconds */
extern volatile uint16_t msec, sec;
extern volatile bool badISR;
extern volatile bool enableDRAMRefresh;

void intrInit(void);

#endif
