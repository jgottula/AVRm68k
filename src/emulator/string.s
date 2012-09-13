/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	/* strings in the text section == PSTR */
	.section .text
	
	
	.global strDbgUserBreak
strDbgUserBreak:
	.asciz "\nUser Break (^C)\n"
	
	.global strDbgCodeBreak
strDbgCodeBreak:
	.asciz "\nCode Break (dbgBreak)\n"
	
	.global strDbgPCDump
strDbgPCDump:
	.asciz "PC = 0x"
	
	.global strDbgCmdPrompt
strDbgCmdPrompt:
	.asciz "dbg] "
	
	.global strDbgBadCmd
strDbgBadCmd:
	.asciz "Bad command!\n"
	
	
	.global strUartTrue
strUartTrue:
	.asciz "true"
	
	.global strUartFalse
strUartFalse:
	.asciz "false"
	
	
	/* ensure that cpu instructions are aligned to word boundaries */
	.align 2
