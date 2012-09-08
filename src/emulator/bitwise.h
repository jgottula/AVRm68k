/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* bitwise utility functions */

#ifndef AVRM68K_EMULATOR_BITWISE_H
#define AVRM68K_EMULATOR_BITWISE_H


uint16_t decodeBigEndian16(const uint8_t *bytes);
uint32_t decodeBigEndian32(const uint8_t *bytes);
uint16_t signExtend8to16(uint8_t byte);
uint32_t signExtend8to32(uint8_t byte);
uint32_t signExtend16to32(uint16_t word);
uint8_t bcdPack(uint8_t msd, uint8_t lsd);
uint16_t bcdUnpack(uint8_t packed);
uint8_t bcdAdd(uint8_t lhs, uint8_t rhs);
uint8_t bcdNegate(uint8_t packed);


/* assembly */
uint32_t shiftRightArithLong(uint32_t operand, uint8_t shifts, uint8_t *flags);
uint32_t shiftLeftArithLong(uint32_t operand, uint8_t shifts, uint8_t *flags);


#endif
