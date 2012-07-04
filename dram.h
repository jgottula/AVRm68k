#ifndef JGOTTULA_DRAM_H
#define JGOTTULA_DRAM_H

#include <stdbool.h>
#include <stdint.h>

uint8_t dramRead(uint32_t addr);
void dramInit(void);

#endif
