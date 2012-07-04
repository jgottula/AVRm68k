#ifndef JGOTTULA_DRAM_H
#define JGOTTULA_DRAM_H

#include <stdbool.h>
#include <stdint.h>

uint8_t dramRead(uint32_t addr);
void dramWrite(uint32_t addr, uint8_t byte);
void dramRefresh(void);
void dramInit(void);

#endif