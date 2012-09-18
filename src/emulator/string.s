/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

#include "../common/macro.s"

	/* strings in the text section == PSTR */
	.section .text
	
	
	global_asciz strDbgUserBreak,"\nUser Break (^C)\n"
	global_asciz strDbgCodeBreak,"\nCode Break (dbgBreak)\n"
	
	global_asciz strDbgPCDump,"PC = 0x"
	global_asciz strDbgCmdPrompt,"dbg] "
	global_asciz strDbgBadCmd,"Syntax error.\n"
	
	.global strDbgHelp
strDbgHelp:
	.ascii "\n"
	.ascii "  command   short  args   description\n"
	.ascii "meta:\n"
	.ascii "  help      h|?           "
		.ascii "this help\n"
	.ascii "system:\n"
	.ascii "  reset                   "
		.ascii "software reset\n"
	.ascii "  die                     "
		.ascii "hard lock-up\n"
	.ascii "control:\n"
	.ascii "  continue  c             "
		.ascii "resume execution\n"
	.ascii "  step      s      [n=1]  "
		.ascii "single step one or more instructions\n"
	.ascii "  next      n      [n=1]  "
		.ascii "same as step, but skip calls\n"
	.ascii "  finish    f             "
		.ascii "run until a ret instruction is executed\n"
	.ascii "  goto      g      hhhh   "
		.ascii "set the program counter\n"
	.ascii "\n"
	.ascii "args:\n"
	.ascii "   x     required\n"
	.ascii "  [x=y]  optional (with default)\n"
	.ascii "integers:\n"
	.ascii "  d      dec number, 0+\n"
	.ascii "  n      dec number, 1+\n"
	.ascii "  hh     hex number, 2 digits\n"
	.ascii "  hhhh   hex number, 4 digits\n"
	.ascii "registers:\n"
	.ascii "  rN     any r[N]    (0<=N<32)\n"
	.ascii "x rW     r[W+1]:r[W] (0<=W<=30, W even)\n"
	.ascii "  rI     X, Y, or Z\n"
	.asciz ""
	
	global_ascii strDbgCmdHelp,"help"
	global_ascii strDbgCmdHelpShort1,"h"
	global_ascii strDbgCmdHelpShort2,"?"
	
	global_ascii strDbgCmdReset,"reset"
	global_ascii strDbgCmdDie,"die"
	
	global_ascii strDbgCmdContinue,"continue"
	global_ascii strDbgCmdContinueShort,"c"
	global_ascii strDbgCmdStep,"step"
	global_ascii strDbgCmdStepShort,"s"
	global_ascii strDbgCmdNext,"next"
	global_ascii strDbgCmdNextShort,"next"
	global_ascii strDbgCmdFinish,"finish"
	global_ascii strDbgCmdFinishShort,"f"
	global_ascii strDbgCmdGoto,"goto"
	global_ascii strDbgCmdGotoShort,"g"
	
	global_asciz strUartTrue,"true"
	global_asciz strUartFalse,"false"
	
	
	/* ensure that cpu instructions are aligned to word boundaries */
	.align 2
