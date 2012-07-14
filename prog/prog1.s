	.section .text
	
	.equ EMU_DUMPREG, 0xa001
	.equ EMU_DUMPMEM, 0xa002
	
	/* init stack ptr */
	lea (0x80),%sp
	
	move.l #0x00000000,%d0
	move.l #0x22222222,%d1
	move.l #0x44444444,%d2
	move.l #0x66666666,%d3
	move.l #0x88888888,%d4
	move.l #0xaaaaaaaa,%d5
	move.l #0xcccccccc,%d6
	move.l #0xeeeeeeee,%d7
	
	movea.l #0x11111111,%a0
	movea.l #0x33333333,%a1
	movea.l #0x55555555,%a2
	movea.l #0x77777777,%a3
	movea.l #0x99999999,%a4
	movea.l #0xbbbbbbbb,%a5
	movea.l #0xdddddddd,%a6
	
	movem.w %d0-%d7/%a0-%a7,-(%sp)
	
	clr.l %d0
	clr.l %d1
	clr.l %d2
	clr.l %d3
	clr.l %d4
	clr.l %d5
	clr.l %d6
	clr.l %d7
	
	movea.l #0x00000000,%a0
	movea.l #0x00000000,%a1
	movea.l #0x00000000,%a2
	movea.l #0x00000000,%a3
	movea.l #0x00000000,%a4
	movea.l #0x00000000,%a5
	movea.l #0x00000000,%a6
	
	movem.w (%sp)+,%a7-%a0/%d7-%d0
	
	.word EMU_DUMPREG
	.word EMU_DUMPMEM
		.long 0x00000000
		.word 0x0080
	
	/* pad out the end */
	.fill 128,1,0xff
