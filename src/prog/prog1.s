/* AVRm68k [test programs]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* instruction tests in m68k assembly */

	.section .text
	
	.equ EMU_DUMPREG, 0xa001
	.equ EMU_DUMPMEM, 0xa002
	
	/* init stack ptr at end of memory */
	lea (0x01000000),%sp
	
	.word EMU_DUMPREG
	.word EMU_DUMPMEM /* vectors */
		.long 0x00000000
		.word 0x0400
	
	move %usp,%a0
	.word EMU_DUMPREG
	move %a0,%usp
	.word EMU_DUMPREG
	
	/* whaaaat? */
	move %usp,%a7
	.word EMU_DUMPREG
	move %a7,%usp
	.word EMU_DUMPREG
	
	move.l #0x10000000,%d0
	move.l #0x00001000,%d1
	move.l #0x00000010,%d2
	
	move.w #0b100000,%ccr
	.word EMU_DUMPREG
	neg.l %d0
	
	move.w #0b100000,%ccr
	.word EMU_DUMPREG
	neg.w %d1
	
	move.w #0b100000,%ccr
	.word EMU_DUMPREG
	neg.b %d2
	
	move.l #0x10000000,%d0
	move.l #0x00001000,%d1
	move.l #0x00000010,%d2
	
	move.w #0b100000,%ccr
	.word EMU_DUMPREG
	negx.l %d0
	
	move.w #0b100000,%ccr
	.word EMU_DUMPREG
	negx.w %d1
	
	move.w #0b100000,%ccr
	.word EMU_DUMPREG
	negx.b %d2
	
	.word EMU_DUMPREG
	.word EMU_DUMPMEM /* stack */
		.long 0x00ffff00
		.word 0x0100
	
	/* pad out the end */
	.fill 128,1,0xff
