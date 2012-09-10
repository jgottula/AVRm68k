/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	.section .text
	
	
	/* parameters:
	 * - uint8_t byte [r24]
	 * no return value
	 */
	.global uartWrite
	.type uartWrite,@function
uartWrite:
	savesreg
	cli
	
uartWrite_WaitLoop:
	lds r0,UCSR0A
	sbrs r0,UDRE0
	jmp uartWrite_WaitLoop
	
	sts UDR0,r24
	
	restsreg
	ret
