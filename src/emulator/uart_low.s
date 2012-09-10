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
	
	
	/* description: writes 'true' or 'false' depending on the value given
	 * parameters:
	 * - bool truth [r24]
	 * no return value
	 */
	.global uartWriteBool
	.type uartWriteBool,@function
uartWriteBool:
	tst r24
	brne uartWriteBool_True
	
	ldi r25,hi8(strUartFalse)
	ldi r24,lo8(strUartFalse)
	jmp uartWritePStr
	
uartWriteBool_True:
	ldi r25,hi8(strUartTrue)
	ldi r24,lo8(strUartTrue)
	jmp uartWritePStr
	
	
	/* description: writes a single hex digit to the UART, optionally uppercase
	 * parameters:
	 * - uint8_t nibble [r24]
	 * - bool    upper  [r22]
	 * no return value
	 */
	.global uartWriteHex4
	.type uartWriteHex4,@function
uartWriteHex4:
	andi r24,0x0f
	cpi r24,0x0a
	brge uartWriteHex4_Letter
	
	ldi r25,'0'
	add r24,r25
	jmp uartWrite
	
uartWriteHex4_Letter:
	ldi r25,('a' - 10)
	sbrc r22,0
	ldi r25,('A' - 10)
	add r24,r25
	jmp uartWrite
	
	
	/* description: writes two hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint8_t byte  [r24]
	 * - bool    upper [r22]
	 * no return value
	 */
	.global uartWriteHex8
	.type uartWriteHex8,@function
uartWriteHex8:
	mov r18,r24
	swap r24
	call uartWriteHex4
	
	mov r24,r18
	jmp uartWriteHex4
	
	
	/* description: writes four hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint16_t word  [r25:r24]
	 * - bool     upper [r22]
	 * no return value
	 */
	.global uartWriteHex16
	.type uartWriteHex16,@function
uartWriteHex16:
	mov r19,r24
	mov r24,r25
	call uartWriteHex8
	
	mov r24,r19
	jmp uartWriteHex8
	
	
	/* description: writes three hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint16_t byteAndAHalf [r25:r24]
	 * - bool     upper        [r22]
	 * no return value
	 */
	.global uartWriteHex12
	.type uartWriteHex12,@function
uartWriteHex12:
	mov r26,r24
	mov r24,r25
	call uartWriteHex4
	
	mov r24,r26
	jmp uartWriteHex8
	
	
	
	/* description: writes six hex digits to the UART, optionally uppercase
	 * parameters:
	 * - uint32_t wordAndAHalf [r25:r22]
	 * - bool     upper        [r20]
	 * no return value
	 */
	.global uartWriteHex24
	.type uartWriteHex24,@function
uartWriteHex24:
	movw r26,r22
	mov r22,r20
	call uartWriteHex8
	
	movw r24,r26
	jmp uartWriteHex16
	
	
	
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
	movw r26,r22
	mov r22,r20
	call uartWriteHex16
	
	movw r24,r26
	jmp uartWriteHex16
