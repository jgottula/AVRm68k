/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

/* convert \n to \r\n for serial terminals */
#define ADD_CARRIAGE_RETURN


	.section .text
	
	
	/* description: atomically writes one byte to the UART
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
	
	
	/* description: writes a string to the UART, adding \r to \n if appropriate
	 * parameters:
	 * - const char *str [r25:r24]
	 * no return value
	 */
	.global uartWriteStr
	.type uartWriteStr,@function
uartWriteStr:
	mov ZH,r25
	mov ZL,r24
	
	ld r24,Z+
	tst r24
	breq uartWriteStr_Done
	
uartWriteStr_Loop:
	call uartWriteChr
	ld r24,Z+
	tst r24
	brne uartWriteStr_Loop
	
uartWriteStr_Done:
	ret


	/* description: same as uartWriteStr, but reads from program memory
	 * parameters:
	 * - const char *pstr [r25:r24]
	 * no return value
	 */
	.global uartWritePStr
	.type uartWritePStr,@function
uartWritePStr:
	mov ZH,r25
	mov ZL,r24
	
	lpm r24,Z+
	tst r24
	breq uartWritePStr_Done
	
uartWritePStr_Loop:
	call uartWriteChr
	lpm r24,Z+
	tst r24
	brne uartWritePStr_Loop
	
uartWritePStr_Done:
	ret
	
	
	/* description: writes an array of given length to the UART
	 * parameters:
	 * - const uint8_t *arr [r25:r24]
	 * - uint8_t        len [r22]
	 * no return value
	 */
	.global uartWriteArr
	.type uartWriteArr,@function
uartWriteArr:
	mov ZH,r25
	mov ZL,r24
	
uartWriteArr_Loop:
	ld r24,Z+
	call uartWrite
	
	dec r22
	brne uartWriteArr_Loop
	
	ret
	
	
	/* description: same as uartWriteArr, but reads from program memory
	 * parameters:
	 * - const uint8_t *parr [r25:r24]
	 * - uint8_t        len [r22]
	 * no return value
	 */
	.global uartWritePArr
	.type uartWritePArr,@function
uartWritePArr:
	mov ZH,r25
	mov ZL,r24
	
uartWritePArr_Loop:
	lpm r24,Z+
	call uartWrite
	
	dec r22
	brne uartWritePArr_Loop
	
	ret
