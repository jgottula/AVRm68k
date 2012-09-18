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
	global_asciz strDbgNotImplemented,"Not implemented.\n"
	
	
	.global strDbgHelp
strDbgHelp:
	.ascii "\n"
	.ascii "  command   short  args   description\n"
	
	.ascii "meta:\n"
	.ascii "  help      h|?                 "
		.ascii "this help\n"
	
	.ascii "system:\n"
	.ascii "  reset                         "
		.ascii "software reset\n"
	.ascii "  die                           "
		.ascii "hard lock-up\n"
	
	.ascii "control:\n"
	.ascii "  continue  c                   "
		.ascii "resume execution\n"
	.ascii "  step      s      [n=1]        "
		.ascii "single step one or more in structions\n"
	.ascii "  next      n      [n=1]       "
		.ascii "same as step, but skip calls\n"
	.ascii "  finish    f                   "
		.ascii "run until a ret instruction is executed\n"
	.ascii "  goto      g      hhhh         "
		.ascii "set the program counter\n"
	
	.ascii "disassembly:\n"
	.ascii "  disasm    d      [n] [hhhh]   "
		.ascii "disassemble the current instruction\n"
	.ascii "                                "
		.ascii "opt: num of instructions to disassemble\n"
	.ascii "                                "
		.ascii "opt: addr at which to disassemble\n"
	
	.ascii "dumping:\n"
	.ascii "  dumpreg   dr     [reg|group]  "
		.ascii "dump all registers, or just the reg/group given\n"
	
	.ascii "\n"
	.ascii "args:\n"
	.ascii "   x       required\n"
	.ascii "  [x=y]    optional (with default)\n"
	
	.ascii "integers:\n"
	.ascii "  d        dec number, 0+\n"
	.ascii "  n        dec number, 1+\n"
	.ascii "  hh       hex number, 2 digits\n"
	.ascii "  hhhh     hex number, 4 digits\n"
	
	.ascii "registers:\n"
	.ascii "  reg      pc, rN, X[H|L], Y[H|L], Z[H|L]\n"
	
	.ascii "register groups:\n"
	.ascii "  all      pc, r[0~25], X, Y, Z\n"
	.ascii "  reg      r[0~31]\n"
	.ascii "  idx      X, Y, Z\n"
	.ascii "  save     call-saved registers: r[2~17], r[28~29]\n"
	.ascii "  clobber  call-used registers: r[18-27], r[30-31]\n"
	.ascii "  fixed    fixed registers: r[0~1]\n"
	.ascii "  return   r[25:22]\n"
	.ascii "  arg      r[25:24], r[23:22], ...\n"
	
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
