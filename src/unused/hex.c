/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

uint8_t hexDecode8(const char hex[2])
{
	uint8_t byte;
	
	if (hex[0] >= '0' && hex[0] <= '9')
		byte = (hex[0] - '0') << 4;
	else
		byte = (0xa + (hex[0] - 'a')) << 4;
	
	if (hex[1] >= '0' && hex[1] <= '9')
		byte |= hex[1] - '0';
	else
		byte |= 0xa + (hex[1] - 'a');
	
	return byte;
}

uint16_t hexDecode16(const char hex[4])
{
	return ((uint16_t)hexDecode8(hex) << 8) | hexDecode8(hex + 2);
}

uint16_t hexDecode32(const char hex[8])
{
	return ((uint32_t)hexDecode16(hex) << 16) | hexDecode16(hex + 4);
}
