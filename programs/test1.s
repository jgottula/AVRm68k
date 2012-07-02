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
	
	/* clr <ea> */
	/*clr.l %d4
	clr.w %d5
	clr.b %d6
	clr.l (%a6)
	clr.w (%a6)
	clr.b (%a6)*
	clr.l (%a6)+
	clr.w (%a6)+
	clr.b (%a6)+
	clr.b (%a7)+*/ /* should increment by 2 */
	clr.l -(%a5)
	clr.w -(%a5)
	clr.b -(%a5)
	clr.b -(%a7)
	
	/*  */
	
	/* next: clr, cmp, eor, or, and */
	/* future: add actual effective addresses for each operation */
