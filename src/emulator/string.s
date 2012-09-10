/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	/* strings in the text section == PSTR */
	.section .text
	
	
	.global strDbgAddr
strDbgAddr:
	.asciz "\nBreak! PC: 0x"
	
	
	/* ensure that cpu instructions are aligned to word boundaries */
	.align 2
