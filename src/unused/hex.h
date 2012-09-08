/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* hexadecimal utility functions */

#ifndef AVRM68K_EMULATOR_HEX_H
#define AVRM68K_EMULATOR_HEX_H


uint8_t hexDecode8(const char hex[2]);
uint16_t hexDecode16(const char hex[4]);
uint16_t hexDecode32(const char hex[8]);


#endif
