/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* dram management and access functions */

#ifndef AVRM68K_EMULATOR_DRAM_H
#define AVRM68K_EMULATOR_DRAM_H


#define DRAM_SAFE_MODE 0


#define DRAM_SIZE         0x2000000 // 2 x (16 x 8 Mbit)
#define DRAM_ROWS         4096      // 4k
#define DRAM_COLS         4096		// 4k
#define DRAM_REFRESH_FREQ 64        // milliseconds


uint8_t dramRead(uint32_t addr);
void dramWrite(uint32_t addr, uint8_t byte);
void dramReadFPM(uint32_t addr, uint16_t len, uint8_t *dest);
void dramWriteFPM(uint32_t addr, uint16_t len, const uint8_t *src);
void dramRefresh(void);
void dramTest(void);
void dramInit(void);


#endif
