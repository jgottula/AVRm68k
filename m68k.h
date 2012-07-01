#ifndef JGOTTULA_M68K_H
#define JGOTTULA_M68K_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#if 0

typedef union {
	uint32_t dword;
	uint16_t word[2];
	uint8_t byte[4];
} Reg;

typedef union {
	uint16_t word;
	uint8_t byte[2];
} Reg16;

#define FLAG_CF _BV(0)
#define FLAG_PF _BV(2)
#define FLAG_AF _BV(4)
#define FLAG_ZF _BV(6)
#define FLAG_SF _BV(7)
#define FLAG_OF _BV(11)

#define SFLAG_TF _BV(8)
#define SFLAG_IF _BV(9)
#define SFLAG_NT _BV(14)
#define SFLAG_RT _BV(16)
#define SFLAG_VM _BV(17)

#define PFX_OPSIZE   _BV(0)
#define PFX_ADDRSIZE _BV(1)

struct {
	struct {
		/* status/instruction */
		Reg eip, eflags;
		
		/* general purpose */
		Reg eax, ebx, ecx, edx;
		Reg ebp, esp, esi, edi;
		
		/* segment */
		Reg16 cs, ss, ds, es, fs, gs;
	} reg;
	
	struct {
		Reg gdtr, ldtr, idtr, tr;
	} mmuReg;
	
	struct {
		Reg cr0, cr1, cr2, cr3;
	} ctrlReg;
	
	/* debug registers not implemented */
	
	/* test registers not implemented */
} cpu;

void i386Next(void);
void i386Init(void);

#endif

#endif
