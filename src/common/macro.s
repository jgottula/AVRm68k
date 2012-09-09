/* AVRm68k [common headers]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

	.altmacro
	
	
	.macro savereg low high
	
	push r\low
	
	.if (\high - \low) != 0
		savereg %(\low + 1),\high
	.endif
	
	.endm
	
	
	.macro restreg high low
	
	pop r\high
	
	.if (\high - \low) != 0
		restreg %(\high - 1),\low
	.endif
	
	.endm
