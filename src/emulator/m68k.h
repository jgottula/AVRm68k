/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* main m68k emulator code */

#ifndef AVRM68K_EMULATOR_M68K_H
#define AVRM68K_EMULATOR_M68K_H


#define SR_CARRY    _BV(1)
#define SR_OVERFLOW _BV(2)
#define SR_ZERO     _BV(3)
#define SR_NEGATIVE _BV(4)
#define SR_EXTEND   _BV(5)


#define SR_INTPRIO0  _BV(8)
#define SR_INTPRIO1  _BV(9)
#define SR_INTPRIO2  _BV(10)
#define SR_INTSTATE  _BV(12)
#define SR_USERSTATE _BV(13)
#define SR_TRACE0    _BV(14)
#define SR_TRACE1    _BV(15)


#define SIZE_BYTE 0b00
#define SIZE_WORD 0b01
#define SIZE_LONG 0b10


#define AMODE_DREGDIRECT   0b000
#define AMODE_AREGDIRECT   0b001
#define AMODE_AREGINDIRECT 0b010
#define AMODE_AREGPOSTINC  0b011
#define AMODE_AREGPREDEC   0b100
#define AMODE_AREGDISPLACE 0b101
#define AMODE_INDEXED      0b110
#define AMODE_EXTRA        0b111


#define AMODE_EXTRA_ABSSHORT   0b000
#define AMODE_EXTRA_ABSLONG    0b001
#define AMODE_EXTRA_PCDISPLACE 0b010
#define AMODE_EXTRA_PCINDEXED  0b011
#define AMODE_EXTRA_IMMEDIATE  0b100


#define SCALE_1 0b00
#define SCALE_2 0b01
#define SCALE_4 0b10
#define SCALE_8 0b11


#define BD_SIZE_NULL 0b01
#define BD_SIZE_WORD 0b10
#define BD_SIZE_LONG 0b11


#define EXT_BRIEF 0
#define EXT_FULL  1


#define SIZE_MOVE_BYTE 0b01
#define SIZE_MOVE_WORD 0b11
#define SIZE_MOVE_LONG 0b10


#define COND_T  0b0000
#define COND_F  0b0001
#define COND_HI 0b0010
#define COND_LS 0b0011
#define COND_CC 0b0100
#define COND_CS 0b0101
#define COND_NE 0b0110
#define COND_EQ 0b0111
#define COND_VC 0b1000
#define COND_VS 0b1001
#define COND_PL 0b1010
#define COND_MI 0b1011
#define COND_GE 0b1100
#define COND_LT 0b1101
#define COND_GT 0b1110
#define COND_LE 0b1111


#define EMUINSTR_DUMPREG 0xa001
#define EMUINSTR_DUMPMEM 0xa002


typedef union {
	uint16_t w;
	uint8_t b[2];
} Reg16;

typedef union {
	uint32_t l;
	uint16_t w[2];
	uint8_t b[4];
} Reg32;

typedef union {
	uint64_t q;
	uint32_t l[2];
	uint16_t w[4];
	uint8_t b[8];
} Reg64;


struct {
	/* user registers */
	struct {
		/* control */
		Reg32 pc;
		
		/* data */
		Reg32 d[8];
		
		/* address */
		Reg32 a[8];
		
		/* fp registers not implemented */
	} ureg;
	
	/* supervisor registers */
	struct {
		/* cache */
		Reg32 caar, cacr;
		
		/* supervisor */
		Reg16 sr, tt0, tt1;
		Reg32 msp, isp, sfc, dfc, vbr;
	} sreg;
	
	/* mmu registers */
	struct {
		Reg16 pmmusr, mmusr;
		Reg32 tc;
		Reg64 crp, srp;
	} mreg;
} cpu;


void m68kDumpReg(void);
void m68kNext(void);
void m68kInit(void);


#endif
