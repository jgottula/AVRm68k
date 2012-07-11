	.section .text
	
	.equ EMU_DUMPREG, 0xa001
	.equ EMU_DUMPMEM, 0xa002
	
	/* init stack ptr */
	lea (0x40),%sp
	
	moveq #-0x40,%d0
	movea.l (0x3c),%a6
	
	moveq #-0x01,%d0
	move.w %d0,%ccr
	move.w %sr,%d1   
	move.w %ccr,%d1
	
	movea.l (0x00000000),%a5
	move.l (0x00000000),%a6
	
	move.l #0x05050505,%d3
	move.b #0x66,%d4
	move.l %d2,%a5
	
	move.l #0xaaaaaaaa,%d6
	not.l %d6
	
	bsr subroutine
	bra done
	
subroutine:
	st  0x00
	sf  0x01
	shi 0x02
	sls 0x03
	scc 0x04
	scs 0x05
	sne 0x06
	seq 0x07
	
	svc 0x08
	svs 0x09
	spl 0x0a
	smi 0x0b
	sge 0x0c
	slt 0x0d
	sgt 0x0e
	sle 0x0f
	
	rts
	
done:
	.word EMU_DUMPREG
	.word EMU_DUMPMEM
		.long 0x00000000
		.word 0x0040
