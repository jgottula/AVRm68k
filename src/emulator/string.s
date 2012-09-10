/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	/* strings in the text section == PSTR */
	.section .text
	
	
	.global strDbgUserBreak
strDbgUserBreak:
	.asciz "\nUser Break (^C)\nPC = 0x"
	
	.global strDbgCodeBreak
strDbgCodeBreak:
	.asciz "\nCode Break (dbgBreak)\nPC = 0x"
	
	
	/* ensure that cpu instructions are aligned to word boundaries */
	.align 2
