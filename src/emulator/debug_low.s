/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	.section .bss
	
	
	.lcomm dbgEnabled,1
	.lcomm dbgForceBreak,1
	
	
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
	
	/* if a break was forced, then jump immediately to the ctrl+c handler */
	lds r18,dbgForceBreak
	tst r18
	breq USART0_RX_vect_GetChar
	jmp USART0_RX_vect_CtrlC
	
USART0_RX_vect_GetChar:
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
	jmp USART0_RX_vect_CtrlC
	
	jmp USART0_RX_vect_Done
	
USART0_RX_vect_CtrlC:
	/* do nothing if debugging is already on */
	lds r18,dbgEnabled
	tst r18
	breq USART0_RX_vect_EnableDebugging
	jmp USART0_RX_vect_Done
	
USART0_RX_vect_EnableDebugging:
	/* enable debugging */
	ldi r18,1
	sts dbgEnabled,r18
	
	/* TODO: turn on timer2 */
	
	/* display the interrupted program counter */
	lds r18,dbgForceBreak
	tst r18
	breq USART0_RX_vect_UserBreak
	ldi r25,hi8(strDbgCodeBreak)
	ldi r24,lo8(strDbgCodeBreak)
	jmp USART0_RX_vect_WritePC
USART0_RX_vect_UserBreak:
	ldi r25,hi8(strDbgUserBreak)
	ldi r24,lo8(strDbgUserBreak)
USART0_RX_vect_WritePC:
	saveclobber
	call uartWritePStr
	restclobber
	ldd r25,Z+1
	ldd r24,Z+2
	reg16_x2 24
	clr r22
	saveclobber
	call uartWriteHex16
	restclobber
	ldi r24,'\n'
	saveclobber
	call uartWriteChr
	restclobber
	
looploop:
	jmp looploop
	
	/*jmp USART0_RX_vect_Done*/
	
USART0_RX_vect_Done:
	/* clear the force break flag */
	sts dbgForceBreak,r1
	
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
	
	
	.global dbgBreak
	.type dbgBreak,@function
dbgBreak:
	cli
	push r18
	ldi r18,1
	sts dbgForceBreak,r18
	pop r18
	jmp _VECTAB(USART0_RX_vect_num)
	
	
	.global dbgStep
	.type dbgStep,@function
dbgStep:
	
	ret
