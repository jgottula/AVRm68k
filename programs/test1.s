	.section .text
	
	/* nop */
	nop
	
	/* exg <an|dn>,<an|dn> */
	exg %d0,%d1
	exg %a0,%a1
	exg %d2,%a2
	
	/* move ccr,<ea> */
	move %ccr,%d0
	move %ccr,%d1
	
	/* moveq #imm,<dn> */
	moveq #0x44,%d0
	moveq #0x00,%d1
	moveq #0x7f,%d2  
	moveq #-0x01,%d3
