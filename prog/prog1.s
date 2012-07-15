	.section .text
	
	.equ EMU_DUMPREG, 0xa001
	.equ EMU_DUMPMEM, 0xa002
	
	/* init stack ptr at end of memory */
	lea (0x01000000),%sp
	
	.word EMU_DUMPREG
	.word EMU_DUMPMEM /* vectors */
		.long 0x00000000
		.word 0x0400
	
	move.l #0xaaaa0004,%d0
	moveq.l #0x00000000,%d1
	
loop:
	addq.l #1,%d1
	move.w %d0,-(%sp)
	dbeq %d0,loop
	
	.word EMU_DUMPREG
	.word EMU_DUMPMEM /* stack */
		.long 0x00ffff00
		.word 0x0100
	
	/* pad out the end */
	.fill 128,1,0xff
