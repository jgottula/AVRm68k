/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"
#include <avr/io.h>

	.section .bss
	
	
dbgEnabled:
	.byte 0
	
	.global dbgChar
dbgChar:
	.byte 0
	
	
	.section .text
	
	
	/* no parameters, no return value */
	.global USART0_RX_vect
	.type USART0_RX_vect,@function
USART0_RX_vect:
	saveclobber
	
	lds r24,UDR0
	
	/* delete me later */
	sts dbgChar,r24
	
	/*call uartWriteChr*/
	
	
#if 0
	cpi r24,0x03 /* etx: ctrl+c */
	breq ...
#endif
	
	restclobber
	reti
	
	
	/* no parameters, no return value */
	.global USART0_TX_vect
	.type USART0_TX_vect,@function
USART0_TX_vect:
	
	reti
	
	
	/* no parameters, no return value */
	.global TIMER2_COMPA_vect
	.type TIMER2_COMPA_vect,@function
TIMER2_COMPA_vect:
	
	reti
	
	
	.global dbgInit
	.type dbgInit,@function
dbgInit:
	
	ret
	
	
	.global dbgStep
	.type dbgStep,@function
dbgStep:
	
	ret
