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
	
	.global strDbgHelp
strDbgHelp:
	.ascii "\n"
	.ascii "  command   short  args  description\n"
	.ascii "meta:\n"
	.ascii "  help      h|?          this help\n"
	.ascii "system:\n"
	.ascii "  reset                  software reset\n"
	.ascii "  die                    hard lock-up\n"
	.ascii "control:\n"
	.ascii "  continue  c            resume execution\n"
	.ascii "  step      s      [n]   single step, optionally n times\n"
	.ascii "  next      n      [n]   same as step, but step over calls\n"
	.ascii "\n"
	.ascii "types:\n"
	.ascii "  d     dec number, 0+\n"
	.ascii "  n     dec number, 1+\n"
	.ascii "  hh    hex number, 2 digits\n"
	.ascii "  hhhh  hex number, 4 digits\n"
	.asciz ""
	
	.global strDbgCmdHelp
strDbgCmdHelp:
	.ascii "help"
	.global strDbgCmdHelpShort1
strDbgCmdHelpShort1:
	.ascii "h"
	.global strDbgCmdHelpShort2
strDbgCmdHelpShort2:
	.ascii "?"
	.global strDbgCmdReset
strDbgCmdReset:
	.ascii "reset"
	.global strDbgCmdDie
strDbgCmdDie:
	.ascii "die"
	
	
	.global strUartTrue
strUartTrue:
	.asciz "true"
	
	.global strUartFalse
strUartFalse:
	.asciz "false"
	
	
	/* ensure that cpu instructions are aligned to word boundaries */
	.align 2
