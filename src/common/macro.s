/* AVRm68k [common headers]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include <avr/io.h>

#define _IO(_x) _SFR_IO_ADDR(_x)
#define _VECTAB(_x) ((_x) * 4)


	.altmacro
	
	
	/* multiply the 16-bit register starting with lsreg by two */
	.macro reg16_x2 lsreg
	.irp reg,lsreg,%(\lsreg + 1)
	
	.if \reg == \lsreg
		add r\reg,r\reg
	.else
		adc r\reg,r\reg
	.endif
	
	.endr
	.endm
	
	
	/* divide the 16-bit register starting with lsreg by two */
	.macro reg16_half lsreg
	.irp reg,%(\lsreg + 1),lsreg
	
	.if \reg != \lsreg
		lsr \reg
	.else
		ror \reg
	.endif
	
	.endr
	.endm
	
	
	/* swaps the contents of the two registers (affects flags) */
	.macro exch reg1 reg2
	
	eor \reg1,\reg2
	eor \reg2,\reg1
	eor \reg1,\reg2
	
	.endm
	
	
	/* push the registers numbered from low to high, inclusive, onto the stack
	 * in ascending order */
	.macro savereg low high
	
	push r\low
	
	.if (\high - \low) != 0
		savereg %(\low + 1),\high
	.endif
	
	.endm
	
	
	/* pop the registers numbered from high to low, inclusive, from the stack in
	 * descending order */
	.macro restreg high low
	
	pop r\high
	
	.if (\high - \low) != 0
		restreg %(\high - 1),\low
	.endif
	
	.endm
	
	
	/* save all registers (excluding r0 and sreg) */
	.macro saveall
	
	savereg 1,31
	
	.endm
	
	
	/* restore all registers (excluding r0 and sreg) */
	.macro restall
	
	restreg 31,1
	
	.endm
	
	
	/* save all call-clobbered registers */
	.macro saveclobber
	
	savereg 18,27
	savereg 30,31
	
	.endm
	
	
	/* restore all call-clobbered registers */
	.macro restclobber
	
	restreg 31,30
	restreg 27,18
	
	.endm
	
	
	/* save the status register (and r0) */
	.macro savesreg
	
	push r0
	in r0,_IO(SREG)
	push r0
	
	.endm
	
	
	/* restore the status register (and r0) */
	.macro restsreg
	
	pop r0
	out _IO(SREG),r0
	pop r0
	
	.endm      
	
	
	/* save the X register (r27:r26) */
	.macro savex
	
	push XH
	push XL
	
	.endm
	
	
	/* restore the X register (r27:r26) */
	.macro restx
	
	pop XL
	pop XH
	
	.endm
	
	
	/* save the Y register (r29:r28) */
	.macro savey
	
	push YH
	push YL
	
	.endm
	
	
	/* restore the Y register (r29:r28) */
	.macro resty
	
	pop YL
	pop YH
	
	.endm
	
	
	/* save the Z register (r31:r30) */
	.macro savez
	
	push ZH
	push ZL
	
	.endm
	
	
	/* restore the Z register (r31:r30) */
	.macro restz
	
	pop ZL
	pop ZH
	
	.endm
	
	
	/* load X with an immediate word */
	.macro loadx imm16
	
	ldi XH,hi8(\imm16)
	ldi XL,lo8(\imm16)
	
	.endm
	
	
	/* load Y with an immediate word */
	.macro loady imm16
	
	ldi YH,hi8(\imm16)
	ldi YL,lo8(\imm16)
	
	.endm
	
	
	/* load Z with an immediate word */
	.macro loadz imm16
	
	ldi ZH,hi8(\imm16)
	ldi ZL,lo8(\imm16)
	
	.endm
	
	
	/* load a register pair with an immediate word */
	.macro load16 regh regl imm16
	
	ldi \regh,hi8(\imm16)
	ldi \regl,lo8(\imm16)
	
	.endm
