/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

/* convert \n to \r\n for serial terminals */
#define ADD_CARRIAGE_RETURN


	.section .text
	
	
	/* description: atomically and synchronously reads a byte from the UART
	 * no parameters
	 * returns:
	 * - uint8_t byte [r24]
	 */
	.global uartRead
	.type uartRead,@function
uartRead:
	/* equivalent of ATOMIC_BLOCK(ATOMIC_RESTORESTATE); needed since the UART is
	 * used by both regular and ISR code */
	savesreg
	cli
	
uartRead_WaitLoop:
	lds r0,UCSR0A
	sbrs r0,RXC0
	jmp uartRead_WaitLoop
	
	lds r24,UDR0
	
	restsreg
	ret
	
	
	/* description: atomically and synchronously writes a byte to the UART
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
	savez
	
	mov ZH,r25
	mov ZL,r24
	
	jmp uartWriteStr_Check
	
uartWriteStr_Loop:
	call uartWriteChr
	
uartWriteStr_Check:
	ld r24,Z+
	tst r24
	brne uartWriteStr_Loop
	
	restz
	ret
	
	
	/* description: same as uartWriteStr, but reads from program memory
	 * parameters:
	 * - const char *pstr [r25:r24]
	 * no return value
	 */
	.global uartWritePStr
	.type uartWritePStr,@function
uartWritePStr:
	savez
	
	mov ZH,r25
	mov ZL,r24
	
	jmp uartWritePStr_Check
	
uartWritePStr_Loop:
	call uartWriteChr
	
uartWritePStr_Check:
	lpm r24,Z+
	tst r24
	brne uartWritePStr_Loop
	
	restz
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
	savez
	
	mov ZH,r25
	mov ZL,r24
	
uartWriteArr_Loop:
	ld r24,Z+
	call uartWrite
	
	dec r22
	brne uartWriteArr_Loop
	
	restz
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
	savez
	
	mov ZH,r25
	mov ZL,r24
	
uartWritePArr_Loop:
	lpm r24,Z+
	call uartWrite
	
	dec r22
	brne uartWritePArr_Loop
	
	restz
	ret
	
	
	/* description: writes 'true' or 'false' depending on the value given
	 * parameters:
	 * - bool truth [r24]
	 * no return value
	 */
	.global uartWriteBool
	.type uartWriteBool,@function
uartWriteBool:
	push r25
	
	tst r24
	brne uartWriteBool_True
	
	ldi r25,hi8(strUartFalse)
	ldi r24,lo8(strUartFalse)
	call uartWritePStr
	
	pop r25
	ret
	
uartWriteBool_True:
	ldi r25,hi8(strUartTrue)
	ldi r24,lo8(strUartTrue)
	call uartWritePStr
	
	pop r25
	ret
	
	
	/* description: writes four binary digits to the UART
	 * parameters:
	 * - uint8_t nibble [r24]
	 * no return value
	 */
	.global uartWriteBin4
	.type uartWriteBin4,@function
uartWriteBin4:
	savereg 15,17
	
	mov r15,r24
	ldi r16,0b1000
	
uartWriteBin4_Loop:
	mov r17,r15
	and r17,r16
	brne uartWriteBin4_One
	
	ldi r24,'0'
	call uartWrite
	
	lsr r16
	brcc uartWriteBin4_Loop
	
	restreg 17,15
	ret
	
uartWriteBin4_One:
	ldi r24,'1'
	call uartWrite
	
	lsr r16
	brcc uartWriteBin4_Loop
	
	restreg 17,15
	ret
	
	
	/* description: writes eight binary digits to the UART
	 * parameters:
	 * - uint8_t byte [r24]
	 * no return value
	 */
	.global uartWriteBin8
	.type uartWriteBin8,@function
uartWriteBin8:
	/* warning, make sure these are the same as for uartWriteBin4, since it will
	 * be doing the restoration */
	savereg 15,17
	
	mov r15,r24
	ldi r16,0b10000000
	
	/* use the 4-bit loop, since all that changed was r18's starting value */
	jmp uartWriteBin4_Loop
	
	
	/* description: writes a single hex digit to the UART, optionally uppercase
	 * parameters:
	 * - uint8_t nibble [r24]
	 * - bool    upper  [r22]
	 * no return value
	 */
	.global uartWriteHex4
	.type uartWriteHex4,@function
uartWriteHex4:
	push r17
	
	andi r24,0x0f
	cpi r24,0x0a
	brsh uartWriteHex4_Letter
	
	ldi r17,'0'
	add r24,r17
	call uartWrite
	
	pop r17
	ret
	
uartWriteHex4_Letter:
	ldi r17,('a' - 10)
	sbrc r22,0
	ldi r17,('A' - 10)
	add r24,r17
	call uartWrite
	
	pop r17
	ret
	
	
	/* description: writes two hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint8_t byte  [r24]
	 * - bool    upper [r22]
	 * no return value
	 */
	.global uartWriteHex8
	.type uartWriteHex8,@function
uartWriteHex8:
	push r17
	
	mov r17,r24
	swap r24
	call uartWriteHex4
	
	mov r24,r17
	call uartWriteHex4
	
	pop r17
	ret
	
	
	/* description: writes four hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint16_t word  [r25:r24]
	 * - bool     upper [r22]
	 * no return value
	 */
	.global uartWriteHex16
	.type uartWriteHex16,@function
uartWriteHex16:
	push r17
	
	mov r17,r24
	mov r24,r25
	call uartWriteHex8
	
	mov r24,r17
	call uartWriteHex8
	
	pop r17
	ret
	
	
	/* description: writes three hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint16_t byteAndAHalf [r25:r24]
	 * - bool     upper        [r22]
	 * no return value
	 */
	.global uartWriteHex12
	.type uartWriteHex12,@function
uartWriteHex12:
	push r17
	
	mov r17,r24
	mov r24,r25
	call uartWriteHex4
	
	mov r24,r17
	call uartWriteHex8
	
	pop r17
	ret
	
	
	
	/* description: writes six hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint32_t wordAndAHalf [r25:r22]
	 * - bool     upper        [r20]
	 * no return value
	 */
	.global uartWriteHex24
	.type uartWriteHex24,@function
uartWriteHex24:
	savereg 16,17
	
	movw r16,r22
	mov r22,r20
	call uartWriteHex8
	
	movw r24,r16
	call uartWriteHex16
	
	restreg 17,16
	ret
	
	
	/* description: same as uartWriteHex24, but only writes hex digits 5:3
	 * parameters:
	 * - uint32_t wordAndAHalf [r25:r22]
	 * - bool     upper        [r20]
	 * no return value
	 */
	.global uartWriteHex24HighHalf
	.type uartWriteHex24HighHalf,@function
uartWriteHex24HighHalf:
	mov r22,r20
	call uartWriteHex8
	
	mov r24,r23
	swap r24
	jmp uartWriteHex4
	
	
	/* description: writes eight hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint32_t dword [r25:r22]
	 * - bool     upper [r20]
	 * no return value
	 */
	.global uartWriteHex32
	.type uartWriteHex32,@function
uartWriteHex32:
	savereg 16,17
	
	movw r16,r22
	mov r22,r20
	call uartWriteHex16
	
	movw r24,r16
	call uartWriteHex16
	
	restreg 17,16
	ret
