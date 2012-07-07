#include "bitwise.h"

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
