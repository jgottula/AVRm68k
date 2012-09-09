/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	.section .text
	
	
	/* parameters:
	 * - uint32_t  operand [r25:r22]
	 * - uint8_t   shifts  [r20]
	 *             (must be >0)
	 * - uint8_t  *flags   [r19:r18]
	 *             (bits 1 and 5 are last bit shifted out)
	 * returns:
	 * - uint32_t result  [r25:r22]
	 */
	.global shiftRightArithLong
	.type shiftRightArithLong,@function
shiftRightArithLong:
	clr r21
	
shiftRightArithLong_Loop:
	/* perform one right-shift */
	asr r25
	ror r24
	ror r23
	ror r22
	
	dec r20
	brne shiftRightArithLong_Loop
	
	/* set flags bits 1 and 5 if last bit shifted was a one */
	brcc shiftRightArithLong_NoCarry
	sbr r21,0b10
	sbr r21,0b100000
	
shiftRightArithLong_NoCarry:
	/* load the flags pointer into Z and store the flags value */
	mov r31,r19
	mov r30,r18
	st Z,r21
	
	ret
	
	
	/* parameters:
	 * - uint32_t  operand [r25:r22]
	 * - uint8_t   shifts  [r20]
	 *             (must be >0)
	 * - uint8_t  *flags   [r19:r18]
	 *             (bits 1 and 5 are last bit shifted out,
	 *              bit 2 indicates if msb changed at any time)
	 * returns:
	 * - uint32_t result  [r25:r22]
	 */
	.global shiftLeftArithLong
	.type shiftLeftArithLong,@function
shiftLeftArithLong:
	clr r21
	
shiftLeftArithLong_Loop:
	/* perform one left-shift */
	lsl r22
	rol r23
	rol r24
	rol r25
	
	/* set flags bit 2 if msb changed during this shift */
	brvc shiftLeftArithLong_NoMSB
	sbr r21,0b100
	
shiftLeftArithLong_NoMSB:
	dec r20
	brne shiftLeftArithLong_Loop
	
	/* set flags bits 1 and 5 if last bit shifted was a one */
	brcc shiftLeftArithLong_NoCarry
	sbr r21,0b10
	sbr r21,0b100000
	
shiftLeftArithLong_NoCarry:
	/* load the flags pointer into Z and store the flags value */
	mov r31,r19
	mov r30,r18
	st Z,r21
	
	ret
