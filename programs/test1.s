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
