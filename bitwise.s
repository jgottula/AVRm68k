	.section .text
	
	
	.global shiftRightArithLong
/* parameters:
 * - uint32_t  operand [r25:r22]
 * - uint8_t   shifts  [r20]
 *             (must be >0)
 * - uint8_t  *flags   [r19:r18]
 *             (changed to zero upon carry, left as is otherwise)
 * returns:
 * - uint32_t result  [r25:r22]
 */
shiftRightArithLong:
	/* perform one right-shift: start with arith right shift so sign bits are
	 * handled appropriately, then rotate all the remaining bits right, with the
	 * last bit ending up in the carry flag */
	asr r25
	ror r24
	ror r23
	ror r22
	
	dec r20
	brne shiftRightArithLong
	
	/* only do the next part if the last shift pushed a one out to carry */
	brcc shiftRightArithLong_Done
	
	/* load the flags pointer into Z */
	mov r31,r19
	mov r30,r18
	
	/* store zero in the byte pointed to by Z */
	st Z,r1
	
shiftRightArithLong_Done:
	ret
