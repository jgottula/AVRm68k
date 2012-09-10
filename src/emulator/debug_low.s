/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	.section .bss
	
	
	.lcomm dbgEnabled,1
	
	
	.section .text
	
	
	/* no parameters, no return value */
	.global USART0_RX_vect
	.type USART0_RX_vect,@function
USART0_RX_vect:
	savesreg
	saveclobber
	
	/* immediately save the stack pointer */
	lds ZH,SPH
	lds ZL,SPL
	adiw ZL,14
	
	/* read the incoming character */
	lds r25,UDR0
	
#if 0
	/* echo the character back */
	ldi r24,'\n'
	call uartWriteChr
	mov r24,r25
	ldi r22,0
	call uartWriteHex8
	ldi r24,'\n'
	call uartWriteChr
#endif
	
	/* ctrl+c */
	cpi r25,0x03
	breq USART0_RX_vect_CtrlC
	
	jmp USART0_RX_vect_Done
	
USART0_RX_vect_CtrlC:
	/* enable debugging */
	ldi r18,1
	sts dbgEnabled,r18
	
	/* display the interrupted program counter */
	ldi r25,hi8(strDbgAddr)
	ldi r24,lo8(strDbgAddr)
	saveclobber
	call uartWritePStr
	restclobber
	ldd r25,Z+1
	ldd r24,Z+2
	reg16_x2 24
	ldi r22,0
	call uartWriteHex16
	ldi r24,'\n'
	call uartWriteChr
	
	/*rjmp USART0_RX_vect_Done*/
	
USART0_RX_vect_Done:
	
	restclobber
	restsreg
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
