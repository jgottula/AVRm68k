/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

uint16_t decodeBigEndian16(const uint8_t *bytes)
{
	return ((uint16_t)bytes[0] << 8) | bytes[1];
}

uint32_t decodeBigEndian32(const uint8_t *bytes)
{
	return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8) | bytes[3];
}

uint16_t signExtend8to16(uint8_t byte)
{
	if ((int8_t)byte < 0)
		return 0xff00 | byte;
	else
		return byte;
}

uint32_t signExtend8to32(uint8_t byte)
{
	if ((int8_t)byte < 0)
		return 0xffffff00 | byte;
	else
		return byte;
}

uint32_t signExtend16to32(uint16_t word)
{
	if ((int16_t)word < 0)
		return 0xffff0000 | word;
	else
		return word;
}

uint8_t bcdPack(uint8_t msd, uint8_t lsd)
{
	return ((msd << 4) | (lsd & 0x0f));
}

uint16_t bcdUnpack(uint8_t packed)
{
	return (((packed & 0xf0) << 4) | (packed & 0x0f));
}

uint8_t bcdAdd(uint8_t lhs, uint8_t rhs)
{
	/* this function assumes valid bcd digits */
	
	/* probably this doesn't work quite right */
	
	uint8_t lsdSum = ((lhs & 0x0f) + (rhs & 0x0f));
	bool carry = (lsdSum >= 10);
	uint8_t msdSum = ((lhs & 0xf0) + (rhs & 0xf0) + (carry ? 0 : 0x10));
	
	return ((msdSum % 160) | (lsdSum % 10));
}

uint8_t bcdNegate(uint8_t packed)
{
	
}
