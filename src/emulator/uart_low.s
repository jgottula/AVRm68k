/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

/* convert \n to \r\n for serial terminals */
#define ADD_CARRIAGE_RETURN


	.section .text
	
	
	/* description: atomically writes one byte to the uart
	 * parameters:
	 * - uint8_t byte [r24]
	 * no return value
	 */
	.global uartWrite
	.type uartWrite,@function
uartWrite:
	/* equivalent of ATOMIC_BLOCK(ATOMIC_RESTORESTATE); needed since the UART is
	 * used by both regular and ISR code */
	savesreg
	cli
	
uartWrite_WaitLoop:
	lds r0,UCSR0A
	sbrs r0,UDRE0
	jmp uartWrite_WaitLoop
	
	sts UDR0,r24
	
	restsreg
	ret
	
	
	/* description: same as uartWrite, but will optionally replace \n with \r\n
	 * parameters:
	 * - uint8_t character [r24]
	 * no return value
	 */
	.global uartWriteChr
	.type uartWriteChr,@function
#ifdef ADD_CARRIAGE_RETURN
uartWriteChr:
	cpi r24,'\n'
	brne uartWrite
	
	ldi r24,'\r'
	call uartWrite
	ldi r24,'\n'
	jmp uartWrite
#else
	uartWriteChr = uartWrite
#endif
