	.section .text
	
	.equ EMU_DUMPREG, 0xa001
	.equ EMU_DUMPMEM, 0xa002
	
	/* init stack ptr */
	lea (0x40),%sp
	
	move.l #0x55555555,%d0
	move.l #0x55555555,%d1
	move.l #0x55555555,%d2
	move.l #0x55555555,%d3
	
	andi.l #0x00000000,%d1
	
	
	/*.word EMU_DUMPREG*/
	
	beq equal
	bne nequal
	nop
	nop
	nop
	.word 0xffff /* die here */
	
equal:
	eor.l #0x00000000,%d0
	bra done
	
nequal:
	and.l #0x00000000,%d0
	bra done
	
done:
	.word EMU_DUMPREG
	.word EMU_DUMPMEM
		.long 0x00000000
		.word 0x0040
