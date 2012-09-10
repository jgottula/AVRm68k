/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "all.h"

void uartWriteDec8(uint8_t byte, bool pad)
{
	bool firstDigit = false;
	char digit;
	
	if (byte < 100)
	{
		if (pad && !firstDigit)
			uartWrite(' ');
	}
	else if (byte < 200)
	{
		uartWrite('1');
		
		firstDigit = true;
		byte -= 100;
	}
	else
	{
		uartWrite('2');
		
		firstDigit = true;
		byte -= 200;
	}
	
	if (byte < 20) /* 0-19 */
	{
		if (byte < 10)
		{
			if (!firstDigit)
			{
				if (pad)
					digit = ' ';
				else
					goto skip10;
			}
			else
				digit = '0';
		}
		else
		{
			digit = '1';
			byte -= 10;
		}
	}
	else /* 20-99 */
	{
		if (byte < 60) /* 20-59 */
		{
			if (byte < 40) /* 20-39 */
			{
				if (byte < 30)
				{
					digit = '2';
					byte -= 20;
				}
				else
				{
					digit = '3';
					byte -= 30;
				}
			}
			else /* 40-59 */
			{
				if (byte < 50)
				{
					digit = '4';
					byte -= 40;
				}
				else
				{
					digit = '5';
					byte -= 50;
				}
			}
		}
		else /* 60-99 */
		{
			if (byte < 80) /* 60-79 */
			{
				if (byte < 70)
				{
					digit = '6';
					byte -= 60;
				}
				else
				{
					digit = '7';
					byte -= 70;
				}
			}
			else /* 80-99 */
			{
				if (byte < 90)
				{
					digit = '8';
					byte -= 80;
				}
				else
				{
					digit = '9';
					byte -= 90;
				}
			}
		}
	}
	
	uartWrite(digit);
	
skip10:
	uartWrite('0' + byte);
}

void uartWriteDec16(uint16_t word, bool pad)
{
	bool firstDigit = false;
	uint8_t quot;
	
	quot = word / 10000;
	if (quot != 0)
	{
		uartWrite('0' + quot);
		
		word -= quot * 10000;
		firstDigit = true;
	}
	else if (pad && !firstDigit)
		uartWrite(' ');
	
	quot = word / 1000;
	if (quot != 0)
	{
		uartWrite('0' + quot);
		
		word -= quot * 1000;
		firstDigit = true;
	}
	else if (pad && !firstDigit)
		uartWrite(' ');
	
	quot = word / 100;
	if (quot != 0)
	{
		uartWrite('0' + quot);
		
		word -= quot * 100;
		firstDigit = true;
	}
	else if (pad && !firstDigit)
		uartWrite(' ');
	
	quot = word / 10;
	if (quot != 0)
	{
		uartWrite('0' + quot);
		
		word -= quot * 10;
		firstDigit = true;
	}
	else if (pad && !firstDigit)
		uartWrite(' ');
	
	uartWrite('0' + quot);
}

bool uartAvail(void)
{
	return (UCSR0A & _BV(RXC0)) != 0;
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
