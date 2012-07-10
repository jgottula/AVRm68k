	.section .text
	
	.equ EMU_DUMPREG, 0xa001
	.equ EMU_DUMPMEM, 0xa002
	
	lea (0x40),%sp
	pea 0xdeadbeef
	
	moveq #-0x40,%d0
	movea.l (0x3c),%a6
	
	moveq #-0x01,%d0
	move.w %d0,%ccr
	move.w %sr,%d1
	
	movea.l (0x00000000),%a5
	move.l (0x00000000),%a5
	
	move.l #0x05050505,%d2
	move.b #0x66,%d2
	move.l %d2,%a2
	
	move.l #0xffffffff,%d0
	not.l %d0
	
	pea 0x0000002e
	rts
	
	jmp 0x00000000
	
	.word EMU_DUMPREG
	.word EMU_DUMPMEM
		.long 0x00000000
		.word 0x0040
