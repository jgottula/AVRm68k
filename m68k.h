#ifndef JGOTTULA_M68K_H
#define JGOTTULA_M68K_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define SR_CARRY    _BV(0)
#define SR_OVERFLOW _BV(1)
#define SR_ZERO     _BV(2)
#define SR_NEGATIVE _BV(3)
#define SR_EXTEND   _BV(4)

#define SR_INTPRIO0  _BV(7)
#define SR_INTPRIO1  _BV(9)
#define SR_INTPRIO2  _BV(8)
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

#define EMUINSTR_DUMPREG 0xa001
#define EMUINSTR_DUMPMEM 0xa002

typedef union {
	uint32_t l;
	uint16_t w[2];
	uint8_t b[4];
} Reg;

struct {
	/* user registers */
	struct {
		/* control */
		Reg pc, sr;
		
		/* data */
		Reg d[8];
		
		/* address */
		Reg a[8];
		
		/* fp registers not implemented */
	} ureg;
	
	/* supervisor registers */
	struct {
		/* cache */
		Reg caar, cacr;
		
		/* dest function code */
		Reg dfc;
		
		/* supervisor */
		Reg msp, sfc, sr, ssp, isp, tt1, tt0, vbr;
	} sreg;
	
	/* mmu registers */
	struct {
		Reg crp, pmmusr, mmusr, srp, tc;
	} mreg;
} cpu;

void m68kDumpReg(void);
void m68kNext(void);
void m68kInit(void);

#endif
