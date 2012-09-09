/* AVRm68k [common headers]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

	.altmacro
	
	
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
	
	
	/* save all call-clobbered registers (only useful for ISRs) */
	.macro saveclobber
	
	savereg 18,27
	savereg 30,31
	
	.endm
	
	
	/* restore all call-clobbered registers (only useful for ISRs) */
	.macro restclobber
	
	restreg 31,30
	restreg 27,18
	
	.endm
