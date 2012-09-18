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
	.asciz "Syntax error.\n"
	
	.global strDbgCmdHelp
strDbgCmdHelp:
	.asciz "help "
	.global strDbgCmdHelpShort1
strDbgCmdHelpShort1:
	.asciz "h "
	.global strDbgCmdHelpShort2
strDbgCmdHelpShort2:
	.asciz "? "
	
	
	.global strUartTrue
strUartTrue:
	.asciz "true"
	
	.global strUartFalse
strUartFalse:
	.asciz "false"
	
	
	/* ensure that cpu instructions are aligned to word boundaries */
	.align 2
