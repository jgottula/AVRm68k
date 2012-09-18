/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	.section .bss
	
	
	.lcomm dbgEnabled,1
	.lcomm dbgForceBreak,1
	.lcomm dbgSingleStep,1
	.lcomm dbgCmdLen,1
	
	
	.section .noinit
	
	
#define DBG_CMD_BUFFER_SIZE 32
	.lcomm dbgCmdBuffer,DBG_CMD_BUFFER_SIZE
	
	
	.section .text
	
	
	/* description: handles ctrl+c as well as subsequent keypresses
	 * no parameters, no return value */
	.global USART0_RX_vect
	.type USART0_RX_vect,@function
USART0_RX_vect:
	push r0
	saveall
	savesreg
	
	/* save the pre-ISR stack pointer */
	lds YH,SPH
	lds YL,SPL
	adiw YL,35
	
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
	brne USART0_RX_vect_CheckNewline
	
	/* if debugging is already on, then ctrl+c should reset the command line */
	lds r18,dbgEnabled
	sbrs r18,0
	jmp USART0_RX_vect_EnableDebugging
	
	ldi r24,'\n'
	call uartWriteChr
	
	jmp USART0_RX_vect_CmdPrompt
	
USART0_RX_vect_CheckNewline:
	/* do nothing if debugging is off */
	lds r18,dbgEnabled
	sbrs r18,0
	jmp USART0_RX_vect_Done
	
	/* if a newline was received, try to parse the command buffer */
	cpi r24,'\r'
	brne USART0_RX_vect_CheckBackspace
	call dbgReadCmd
	
	/* clear the command buffer */
	sts dbgCmdLen,r1
	
	/* continue the command prompt only if the return value is zero */
	sbrs r24,0
	jmp USART0_RX_vect_CmdPrompt
	
	jmp USART0_RX_vect_Done
	
USART0_RX_vect_CheckBackspace:
	/* if a backspace was received, delete a character */
	cpi r24,0x08
	brne USART0_RX_vect_NotBackspace
	
	lds r18,dbgCmdLen
	tst r18
	breq USART0_RX_vect_BackspaceDone
	
	/* decrease the command buffer length by one and echo the backspace */
	dec r18
	sts dbgCmdLen,r18
	
	call uartWrite
	ldi r24,' '
	call uartWrite
	ldi r24,0x08
	call uartWrite
	
USART0_RX_vect_BackspaceDone:
	jmp USART0_RX_vect_CmdWait
	
USART0_RX_vect_NotBackspace:
	/* ignore non-printable characters: 0-31, 127, 128-255 */
	cpi r24,' '
	brsh USART0_RX_vect_Printable0
	
	jmp USART0_RX_vect_CmdWait
	
USART0_RX_vect_Printable0:
	cpi r24,0x7f
	brlo USART0_RX_vect_Printable1
	
	jmp USART0_RX_vect_CmdWait
	
USART0_RX_vect_Printable1:
	/* do nothing if the command buffer is full */
	lds r18,dbgCmdLen
	cpi r18,DBG_CMD_BUFFER_SIZE
	brlo USART0_RX_vect_AppendToBuffer
	
	jmp USART0_RX_vect_CmdWait
	
USART0_RX_vect_AppendToBuffer:
	/* append the character to the buffer and echo it */
	loadz dbgCmdBuffer
	
	add ZL,r18
	adc ZH,0
	
	inc r18
	sts dbgCmdLen,r18
	
	st Z,r24
	
	call uartWrite
	
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
	
	load16 r25,r24,strDbgCodeBreak
	
	/* clear the force break flag */
	sts dbgForceBreak,r1
	jmp USART0_RX_vect_WriteBreak
	
USART0_RX_vect_UserBreak:
	load16 r25,r24,strDbgUserBreak
	
USART0_RX_vect_WriteBreak:
	call uartWritePStr
	
USART0_RX_vect_WritePC:
	load16 r25,r24,strDbgPCDump
	call uartWritePStr
	
	ldd r25,Y+1
	ldd r24,Y+2
	reg16_x2 24
	clr r22
	call uartWriteHex16
	ldi r24,'\n'
	call uartWriteChr
	
USART0_RX_vect_CmdPrompt:
	sts dbgCmdLen,r1
	
	load16 r25,r24,strDbgCmdPrompt
	call uartWritePStr
	
USART0_RX_vect_CmdWait:
	call uartRead
	jmp USART0_RX_vect_GetChar
	
USART0_RX_vect_Done:
	ldi r24,'\n'
	call uartWriteChr
	
	restsreg
	restall
	pop r0
	reti
	
	
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
	
	
	/* description: parses the command string when a newline is received
	 * no parameters, no return value
	 * not public
	 */
	.type dbgReadCmd,@function
dbgReadCmd:
	savez
	push r18
	push r25
	
	ldi r24,'\n'
	call uartWriteChr
	
	/* do nothing if the buffer is empty */
	lds r18,dbgCmdLen
	tst r18
	brne dbgReadCmd_CheckHelp
	jmp dbgReadCmd_DonePrompt
	
dbgReadCmd_CheckHelp:
	loadz strDbgCmdHelp
	ldi r24,4
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecHelp
	
	loadz strDbgCmdHelpShort1
	ldi r24,1
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecHelp
	
	loadz strDbgCmdHelpShort2
	ldi r24,1
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecHelp
	
	jmp dbgReadCmd_CheckReset
	
dbgReadCmd_ExecHelp:
	load16 r25,r24,strDbgHelp
	call uartWritePStr
	
	jmp dbgReadCmd_DonePrompt
	
dbgReadCmd_CheckReset:
	loadz strDbgCmdReset
	ldi r24,5
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecReset
	
	jmp dbgReadCmd_CheckDie
	
dbgReadCmd_ExecReset:
	jmp reset
	
dbgReadCmd_CheckDie:
	loadz strDbgCmdDie
	ldi r24,3
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecDie
	
	jmp dbgReadCmd_CheckContinue
	
dbgReadCmd_ExecDie:
	/* disable interrupts and loop forever */
	cli
	
dbgReadCmd_ExecDie_Loop:
	jmp dbgReadCmd_ExecDie_Loop
	
dbgReadCmd_CheckContinue:
	loadz strDbgCmdContinue
	ldi r24,8
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecContinue
	
	loadz strDbgCmdContinueShort
	ldi r24,1
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecContinue
	
	jmp dbgReadCmd_CheckStep
	
dbgReadCmd_ExecContinue:
	/* disable debugging, turn off TIMER2, and clear its overflow flag */
	sts dbgEnabled,r1
	sts TIMSK2,r1
	ldi r18,0xff
	sts TIFR2,r18
	
	jmp dbgReadCmd_ExitISR
	
dbgReadCmd_CheckStep:
	loadz strDbgCmdStep
	ldi r24,4
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecStep
	
	loadz strDbgCmdStepShort
	ldi r24,1
	call dbgCheckCmd
	tst r24
	breq dbgReadCmd_ExecStep
	
	jmp dbgReadCmd_CheckNext
	
dbgReadCmd_ExecStep:
	jmp dbgReadCmd_ExitISR
	
dbgReadCmd_CheckNext:
	
	
	
	
	
dbgReadCmd_CheckR:
	cpi r24,'r'
	brne dbgReadCmd_BadCmd
	
	call dbgDumpReg
	
	jmp dbgReadCmd_DonePrompt
	
dbgReadCmd_BadCmd:
	ldi r25,hi8(strDbgBadCmd)
	ldi r24,lo8(strDbgBadCmd)
	call uartWritePStr
	
	jmp dbgReadCmd_DonePrompt
	
dbgReadCmd_DonePrompt:
	clr r24
	
	pop r18
	pop r25
	restz
	ret
	
dbgReadCmd_ExitISR:
	ldi r24,1
	
	pop r18
	pop r25
	restz
	ret
	
	
	/* description: dumps registers (on the stack at Y) to the UART
	 * parameters:
	 * - Z (pre-ISR stack pointer)
	 * no return value
	 * not public
	 */
	.type dbgDumpReg,@function
dbgDumpReg:
	savey
	push r22
	push r24
	
	sbiw YL,31
	
	.macro dumpreg_forreal offset
	
	ldd r24,Y+\offset
	call uartWriteHex8
	
	.endm
	
	.macro dumpreg reg
	
	.if (\reg < 10) && ((\reg % 2) != 0)
		ldi r24,' '
		call uartWrite
	.endif
	
	.if (\reg % 2) != 0
		ldi r24,'r'
		call uartWrite
	.endif
	
	.if \reg >= 10
		ldi r24,('0' + (\reg / 10))
		call uartWrite
	.endif
	
	ldi r24,('0' + (\reg % 10))
	call uartWrite
	
	.if (\reg % 2) == 0
		ldi r24,' '
		call uartWrite
		
		.if \reg < 10
			call uartWrite
		.endif
		
		ldi r22,0
		
		dumpreg_forreal %(31 - \reg)
		dumpreg_forreal %(30 - \reg)
		
		ldi r24,'\n'
		call uartWriteChr
	.else
		ldi r24,':'
		call uartWrite
		
		dumpreg %(\reg - 1)
	.endif
	
	.endm
	
	.irp reg,31,29,27,25,23,21,19,17,15,13,11,9,7,5,3,1
		dumpreg \reg
	.endr
	
	pop r24
	pop r22
	resty
	ret
	
	
	/* description: checks the command string against a pstr
	 * parameters:
	 * - Z (pstr addr) [not saved]
	 * - r18 (str length) [saved]
	 * - r24 (pstr length) [cannot be zero]
	 * returns: r24 (nonzero if not matched)
	 * not public
	 */
	.type dbgCheckCmd,@function
dbgCheckCmd:
	cp r18,r24
	breq dbgCheckCmd_Check
	
	/* r24 must be nonzero at this point */
	ret
	
dbgCheckCmd_Check:
	push r20
	push r21
	savex
	
	loadx dbgCmdBuffer
	
dbgCheckCmd_Loop:
	ld r20,X+
	lpm r21,Z+
	
	cp r20,r21
	brne dbgCheckCmd_Done
	
	dec r24
	brne dbgCheckCmd_Loop
	
	/* if we made it successfully thru the loop, r24 will be zero */
	
dbgCheckCmd_Done:
	pop r21
	pop r20
	restx
	ret
