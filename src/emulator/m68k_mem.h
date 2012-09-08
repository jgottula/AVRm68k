/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* m68k memory access code */

#ifndef AVRM68K_EMULATOR_M68K_MEM_H
#define AVRM68K_EMULATOR_M68K_MEM_H


uint32_t memRead(uint32_t addr, uint8_t size);
void memWrite(uint32_t addr, uint8_t size, uint32_t value);
void memDump(uint32_t addr, uint16_t len);


#endif
