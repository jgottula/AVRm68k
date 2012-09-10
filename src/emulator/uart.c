/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

bool uartAvail(void)
{
	return (UCSR0A & _BV(RXC0)) != 0;
}

void uartWriteDec16(uint16_t word)
{
	int8_t i = 4;
	bool firstDigit = false;
	
	for ( ; i >= 0; --i)
	{
		uint16_t temp = word;
		
		/* hack for 0 */
		if (i == 0)
			firstDigit = true;
		
		for (int8_t j = 0; j < i; ++j)
			temp /= 10;
		
		/* modulo is expensive, so do it last */
		temp %= 10;
		
		/* don't print leading zeros */
		if (temp == 0 && !firstDigit)
			continue;
		
		uartWrite('0' + temp);
		firstDigit = true;
	}
}

void uartWriteHex16(uint16_t word, bool upper)
{
	uartWriteHex8(word >> 8, upper);
	uartWriteHex8(word, upper);
}

void uartWriteHexTop12(uint32_t wordAndAHalf, bool upper)
{
	/* this function takes a 24-bit value and prints the high 12 bits */
	uartWriteHex4(wordAndAHalf >> 20, upper);
	uartWriteHex4(wordAndAHalf >> 16, upper);
	uartWriteHex4(wordAndAHalf >> 12, upper);
}

void uartWriteHexBottom12(uint32_t wordAndAHalf, bool upper)
{
	/* this function takes a 24-bit value and prints the low 12 bits */
	uartWriteHex4(wordAndAHalf >> 8, upper);
	uartWriteHex4(wordAndAHalf >> 4, upper);
	uartWriteHex4(wordAndAHalf, upper);
}

void uartWriteHex24(uint32_t wordAndAHalf, bool upper)
{
	uartWriteHex8(wordAndAHalf >> 16, upper);
	uartWriteHex8(wordAndAHalf >> 8, upper);
	uartWriteHex8(wordAndAHalf, upper);
}

void uartWriteHex32(uint32_t dword, bool upper)
{
	uartWriteHex16(dword >> 16, upper);
	uartWriteHex16(dword, upper);
}

void uartWriteHex256(const uint32_t arr[8], bool upper)
{
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (i != 0)
			uartWrite(' ');
		uartWriteHex32(arr[i], upper);
	}
}

void uartWriteBin4(uint8_t nibble)
{
	for (int8_t i = 3; i >= 0; --i)
	{
		if ((nibble >> i) & 0b1)
			uartWrite('1');
		else
			uartWrite('0');
	}
}

void uartWriteBin8(uint8_t byte)
{
	for (int8_t i = 7; i >= 0; --i)
	{
		if ((byte >> i) & 0b1)
			uartWrite('1');
		else
			uartWrite('0');
	}
}

void uartWriteBool(bool truth)
{
	if (truth)
		uartWritePSTR("TRUE");
	else
		uartWritePSTR("false");
}

bool uartEnabled(void)
{
	return UCSR0B & _BV(TXEN0);
}

void uartInit(void)
{
	/* set the baud rate to 38400 baud
	 * the formula is: (F_CPU / (8 * BAUD_RATE)) - 1 [with U2X0 on]
	 * see table 19-12 in the ATmega1284P manual for details */
	UBRR0 = 64;
	
	/* disable the double speed bit */
	writeIO(&UCSR0A, _BV(MPCM0) | _BV(U2X0), 0);
	
	/* set the UART to asynchronous mode with no parity, one stop bit, and 8-bit
	 * character size */
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
	
	/* enable the UART with receive and transmit complete interrupts enabled */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0) | _BV(TXCIE0);
}
