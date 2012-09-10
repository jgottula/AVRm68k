/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	.section .bss
	
	
	.lcomm dbgEnabled,1
	.lcomm dbgForceBreak,1
	.lcomm dbgSingleStep,1
	
	
	.section .text
	
	
	/* description: handles ctrl+c as well as subsequent keypresses
	 * no parameters, no return value */
	.global USART0_RX_vect
	.type USART0_RX_vect,@function
USART0_RX_vect:
	savesreg
	saveclobber
	
	/* immediately save the stack pointer */
	lds ZH,SPH
	lds ZL,SPL
	adiw ZL,14
	
	/* if we just came back from a single step, then skip to the PC dump */
	lds r18,dbgSingleStep
	tst r18
	breq USART0_RX_vect_NotSingleStep
	sts dbgSingleStep,r1
	jmp USART0_RX_vect_WritePC
	
USART0_RX_vect_NotSingleStep:
	/* if a break was forced, then skip the serial character handling */
	lds r18,dbgForceBreak
	sbrc r18,0
	jmp USART0_RX_vect_EnableDebugging
	
	/* read the incoming character */
	lds r24,UDR0
	
USART0_RX_vect_GetChar:
	/* ctrl+c */
	cpi r24,0x03
	brne USART0_RX_vect_CheckLetters
	
	/* do nothing if debugging is already on */
	lds r18,dbgEnabled
	sbrs r18,0
	jmp USART0_RX_vect_EnableDebugging
	jmp USART0_RX_vect_CmdWait
	
USART0_RX_vect_CheckLetters:
	/* do nothing if debugging is off */
	lds r18,dbgEnabled
	sbrs r18,0
	jmp USART0_RX_vect_Done
	
USART0_RX_vect_CheckS:
	/* single step */
	cpi r24,'s'
	brne USART0_RX_vect_CheckC
	
	jmp USART0_RX_vect_Done
	
USART0_RX_vect_CheckC:
	/* continue execution */
	cpi r24,'c'
	brne USART0_RX_vect_CheckOthers
	
	/* disable debugging, turn off TIMER2, and clear its overflow flag */
	sts dbgEnabled,r1
	sts TIMSK2,r1
	ldi r18,0xff
	sts TIFR2,r18
	
	jmp USART0_RX_vect_Done
	
USART0_RX_vect_CheckOthers:
	/* ignore other characters */
	jmp USART0_RX_vect_CmdWait
	
USART0_RX_vect_EnableDebugging:
	/* set the debugging flag */
	ldi r18,1
	sts dbgEnabled,r18
	
	/* enable interrupt upon TIMER2 overflow (TCNT2 == OCR2A) */
	ldi r18,_BV(OCIE2A)
	sts TIMSK2,r18
	
	/* display the interrupted program counter */
	lds r18,dbgForceBreak
	tst r18
	breq USART0_RX_vect_UserBreak
	
	ldi r25,hi8(strDbgCodeBreak)
	ldi r24,lo8(strDbgCodeBreak)
	
	/* clear the force break flag */
	sts dbgForceBreak,r1
	jmp USART0_RX_vect_WriteBreak
	
USART0_RX_vect_UserBreak:
	ldi r25,hi8(strDbgUserBreak)
	ldi r24,lo8(strDbgUserBreak)
	
USART0_RX_vect_WriteBreak:
	call uartWritePStr
	
USART0_RX_vect_WritePC:
	ldi r25,hi8(strDbgPCDump)
	ldi r24,lo8(strDbgPCDump)
	call uartWritePStr
	
	ldd r25,Z+1
	ldd r24,Z+2
	reg16_x2 24
	clr r22
	call uartWriteHex16
	ldi r24,'\n'
	call uartWriteChr
	
USART0_RX_vect_CmdPrompt:
	ldi r25,hi8(strDbgCmdPrompt)
	ldi r24,lo8(strDbgCmdPrompt)
	call uartWritePStr
	
USART0_RX_vect_CmdWait:
	call uartRead
	jmp USART0_RX_vect_GetChar
	
USART0_RX_vect_Done:
	ldi r24,'\n'
	call uartWriteChr
	
	restclobber
	restsreg
	reti
	/* PROBLEM: by reading the UART input with a spin loop inside of the rx isr,
	 * we may be causing an rx interrupt to queue up, which might run the isr an
	 * extra, unwanted time */
	
	
	/* description: currently unused
	 * no parameters, no return value */
	.global USART0_TX_vect
	.type USART0_TX_vect,@function
USART0_TX_vect:
	
	reti
	
	
	/* description: returns to the debug prompt after a single-step operation
	 * no parameters, no return value */
	.global TIMER2_COMPA_vect
	.type TIMER2_COMPA_vect,@function
TIMER2_COMPA_vect:
	/* check if debugging is enabled, and exit if it is not */
	push r18
	lds r18,dbgEnabled
	tst r18
	brne TIMER2_COMPA_vect_Break
	
	pop r18
	reti
	
TIMER2_COMPA_vect_Break:
	ldi r18,1
	sts dbgSingleStep,r18
	pop r18
	jmp _VECTAB(USART0_RX_vect_num)
	
	
	/* description: sets up TIMER2 without starting it
	 * no parameters, no return value */
	.global dbgInit
	.type dbgInit,@function
dbgInit:
	/* set TIMER2 to run at F_CPU Hz and overflow when equal to OCR2A */
	ldi r18,(_BV(CS20) | _BV(WGM22))
	sts TCCR2B,r18
	
	/* set the overflow threshold to 1 (always overflow) */
	ldi r18,1
	sts OCR2A,r18
	
	/* zero out the counter */
	sts TCNT2,r1
	
	ret
	
	
	/* description: triggers a debug break from within normal user code
	 * no parameters, no return value */
	.global dbgBreak
	.type dbgBreak,@function
dbgBreak:
	cli
	push r18
	ldi r18,1
	sts dbgForceBreak,r18
	pop r18
	jmp _VECTAB(USART0_RX_vect_num)
