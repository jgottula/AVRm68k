	.section .text
	
	.equ EMU_DUMPREG, 0xa001
	.equ EMU_DUMPMEM, 0xa002
	
	/* init stack ptr at end of memory */
	lea (0x01000000),%sp
	
	.word EMU_DUMPREG
	.word EMU_DUMPMEM /* vectors */
		.long 0x00000000
		.word 0x0400
	
	movea.l #0x00ffff00,%a0   
	movea.l #0x00ffff04,%a1
	
	move.l #0x55555555,(%a0)
	move.l #0xaaaaaaaa,(%a1)
	
	btst.l #0x00,(%a0)
	.word EMU_DUMPREG
	
	move.l #0xffffff00,%d1
	btst.l %d1,(%a1)
	.word EMU_DUMPREG
	
	.word EMU_DUMPREG
	.word EMU_DUMPMEM /* stack */
		.long 0x00ffff00
		.word 0x0100
	
	/* pad out the end */
	.fill 128,1,0xff
