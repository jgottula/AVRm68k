#include "uart.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "debug.h"
#include "io.h"

ISR(USART0_RX_vect)
{
	/* tell the other device to wait until we've read this */
	PORT_UART &= ~UART_RTS;
}

bool uartAvail(void)
{
	return (UCSR0A & _BV(RXC0)) != 0;
}

uint8_t uartRead(void)
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	
	uint8_t byte = UDR0;
	
	/* clear the other device to write to us */
	PORT_UART |= UART_RTS;
	
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
		
		uartWriteChr('0' + temp);
		firstDigit = true;
	}
}

void uartWriteHex4(uint8_t nibble, bool upper)
{
	nibble &= 0xf;
	
	if (nibble < 0xa)
		uartWriteChr('0' + nibble);
	else
		uartWriteChr((upper ? 'A' : 'a') + (nibble - 0xa));
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
			uartWriteChr(' ');
		uartWriteHex32(arr[i], upper);
	}
}

void uartWriteBin4(uint8_t nibble)
{
	for (int8_t i = 3; i >= 0; --i)
	{
		if ((nibble >> i) & 0b1)
			uartWriteChr('1');
		else
			uartWriteChr('0');
	}
}

void uartWriteBin8(uint8_t byte)
{
	for (int8_t i = 7; i >= 0; --i)
	{
		if ((byte >> i) & 0b1)
			uartWriteChr('1');
		else
			uartWriteChr('0');
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
	
	uartWrite((uint8_t)chr);
}

void uartWrite(uint8_t byte)
{
	/* wait for the receiver if necessary */
	while ((PIN_UART & UART_CTS) == 0);
	
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = byte;
}

bool uartEnabled(void)
{
	return UCSR0B & _BV(TXEN0);
}

void uartInit(void)
{
	/* set cts and rts modes; deassert rts and enable pullup on cts */
	writeIO(&DDR_UART, UART_CTS | UART_RTS, UART_RTS);
	writeIO(&PORT_UART, UART_CTS | UART_RTS, UART_CTS | UART_RTS);
	
	/* set the baud rate to 1.25 Mbaud
	 * the formula is: (F_CPU / (8 * BAUD_RATE)) - 1 [with U2X0 on]
	 * see table 20-11 in the ATmega328p manual for details */
	UBRR0 = 1;
	
	/* enable the double speed bit */
	writeIO(&UCSR0A, _BV(MPCM0) | _BV(U2X0), _BV(U2X0));
	
	/* set the UART to asynchronous mode with no parity, one stop bit, and 8-bit
	 * character size */
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
	
	/* enable the UART with the rx interrupt on */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
}
