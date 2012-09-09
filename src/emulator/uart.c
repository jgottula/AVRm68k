/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

bool uartAvail(void)
{
	return (UCSR0A & _BV(RXC0)) != 0;
}

uint8_t uartRead(void)
{
	uint8_t byte;
	
	/* RESTORESTATE is used since this function may be used in an ISR, in which
	 * case interrupts should remain disabled until the ISR is done */
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		loop_until_bit_is_set(UCSR0A, RXC0);
		byte = UDR0;
	}
	
	return byte;
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

void uartWriteHex4(uint8_t nibble, bool upper)
{
	nibble &= 0xf;
	
	if (nibble < 0xa)
		uartWrite('0' + nibble);
	else
		uartWrite((upper ? 'A' : 'a') + (nibble - 0xa));
}

void uartWriteHex8(uint8_t byte, bool upper)
{
	uartWriteHex4(byte >> 4, upper);
	uartWriteHex4(byte, upper);
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

void uartWriteArr(const uint8_t *arr, uint16_t len)
{
	do
		uartWrite(*(arr++));
	while (--len > 0);
}

void uartWritePArr(const uint8_t *arr, uint16_t len)
{
	do
		uartWrite(pgm_read_byte(arr++));
	while (--len > 0);
}

void uartWriteStr(const char *str)
{
	/* don't write the terminating null */
	while (*str != 0)
		uartWriteChr(*(str++));
}

void uartWritePStr(const char *str)
{
	char c;
	
	/* don't write the terminating null */
	while ((c = pgm_read_byte(str++)) != 0)
		uartWriteChr(c);
}

void uartWriteChr(char chr)
{
#ifdef ADD_CARRIAGE_RETURN
	if (chr == '\n')
		uartWrite('\r');
#endif
	
	uartWrite(chr);
}

void uartWrite(uint8_t byte)
{
	/* bug report:
	 * 
	 * this function had issues when the ATOMIC_BLOCK was introduced, but only
	 * when compiled with -O2 (worked with -O1, and strangely enough, -O3 as
	 * well); the fix involved making all the functions that know they won't
	 * need newline replacement use uartWrite instead of uartWriteChr, which
	 * apparently caused the compiler to inline things in a better way at -O2,
	 * thereby fixing the problem
	 * 
	 * I have no idea exactly what was wrong or why this particular fix worked,
	 * so don't ask */
	
	/* RESTORESTATE is used since this function may be used in an ISR, in which
	 * case interrupts should remain disabled until the ISR is done */
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		loop_until_bit_is_set(UCSR0A, UDRE0);
		
		UDR0 = byte;
	}
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
