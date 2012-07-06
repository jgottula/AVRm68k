#include "shift.h"
#include "io.h"

#if 0

static void shiftLatch()
{
	SHIFT_PORT ^= SHIFT_LATCH;
	SHIFT_PORT ^= SHIFT_LATCH;
}

static void shiftZeroNoLatch()
{
	SHIFT_PORT ^= SHIFT_CLEAR;
	SHIFT_PORT ^= SHIFT_CLEAR;
}

static void shiftOut(uint8_t byte)
{
	for (uint8_t i = 0; i < 8; ++i)
	{
		/* load data */
		writeIO(&SHIFT_PORT, SHIFT_DATA, ((byte & _BV(1)) ? SHIFT_DATA : 0));
		SHIFT_PORT ^= SHIFT_CLK;
		SHIFT_PORT ^= SHIFT_CLK;
		
		/* move to next bit */
		byte = (byte >> 1);
	}
}

void shiftOut8(uint8_t byte)
{
	shiftOut(byte);
	shiftLatch();
}

void shiftOut16(uint16_t word)
{
	shiftOut(word);
	shiftOut(word >> 8);
	shiftLatch();
}

void shiftOut24(uint32_t dword)
{
	shiftOut(dword);
	shiftOut(dword >> 8);
	shiftOut(dword >> 16);
	shiftLatch();
}

void shiftOut32(uint32_t dword)
{
	shiftOut(dword);
	shiftOut(dword >> 8);
	shiftOut(dword >> 16);
	shiftOut(dword >> 24);
	shiftLatch();
}

void shiftZero(void)
{
	shiftZeroNoLatch();
	shiftLatch();
}

void shiftInit(void)
{
	writeIO(&SHIFT_PORT, SHIFT_ALL, SHIFT_CLK | SHIFT_LATCH | SHIFT_CLEAR);
}

#endif
