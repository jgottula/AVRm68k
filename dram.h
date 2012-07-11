#ifndef JGOTTULA_DRAM_H
#define JGOTTULA_DRAM_H

#include <stdbool.h>
#include <stdint.h>

#define DRAM_SAFE_MODE 0

#define DRAM_SIZE         0x1000000 // 16 x 8 Mbit
#define DRAM_REFRESH_FREQ 64        // milliseconds
#define DRAM_REFRESH_ROWS 4096      // 4k

uint8_t dramRead(uint32_t addr);
void dramWrite(uint32_t addr, uint8_t byte);
void dramRefresh(void);
void dramTest(void);
void dramInit(void);

#endif
