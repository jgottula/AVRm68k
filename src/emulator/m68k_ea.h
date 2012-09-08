/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* m68k effective address calculations */

#ifndef AVRM68K_EMULATOR_M68K_EA_H
#define AVRM68K_EMULATOR_M68K_EA_H


uint32_t accessEA(const uint8_t *ptr, uint32_t addr, uint8_t mode, uint8_t reg,
	uint32_t data, uint8_t size, bool write);
uint8_t calcEA(const uint8_t *ptr, uint8_t mode, uint8_t reg, uint8_t size,
	uint32_t *addrOut);


#endif
