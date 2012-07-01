#include "m68k_instr.h"
#include "debug.h"
#include "m68k.h"
#include "m68k_mem.h"
#include "uart.h"

#if 0

extern uint8_t *instr;
extern uint8_t prefix;

/* order matters! see 80386 manual page 246 */
Reg *gpReg[] = { &cpu.reg.eax, &cpu.reg.ecx, &cpu.reg.edx, &cpu.reg.ebx,
	&cpu.reg.esp, &cpu.reg.ebp, &cpu.reg.esi, &cpu.reg.edi };

void instrMOV(void)
{
	dbgHeader();
	uartWritePSTR("mov\n");
	
	if (instr[0] >= 0xb0 && instr[0] <= 0xb7) // mov imm8,reg8
	{
		cpu.reg.eip.dword += 2;
		
		Reg *dest = gpReg[(instr[0] - 0xb0) % 4];
		uint8_t imm8 = instr[1];
		bool high = (instr[0] - 0xb0 >= 4);
		
		if (high)
			dest->byte[1] = imm8;
		else
			dest->byte[0] = imm8;
	}
	else if (instr[0] >= 0xb8 && instr[0] <= 0xbf) // mov imm16|32,reg16|32
	{
		Reg *dest = gpReg[instr[0] - 0xb8];
		
		if (prefix & PFX_OPSIZE) // mov imm16,reg16
		{
			cpu.reg.eip.dword += 4;
			
			uint16_t imm16 = instr[1] | (instr[2] << 8);
			dest->word[0] = imm16;
		}
		else // mov imm32,reg32
		{
			cpu.reg.eip.dword += 5;
			
			uint32_t imm32 = instr[1] | ((uint32_t)instr[2] << 8) |
				((uint32_t)instr[3] << 16) | ((uint32_t)instr[4] << 24);
			dest->dword = imm32;
		}
	}
	else
		assert(0);
	
	/* MOV does not affect flags */
}

void instrXOR(void)
{
	uint32_t result;
	uint8_t resultSize = 0;
	
	dbgHeader();
	uartWritePSTR("xor\n");
	
	if (instr[0] == 0x34) // xor al,imm8
	{
		cpu.reg.eip.dword += 2;
		
		uint8_t imm8 = instr[1];
		
		cpu.reg.eax.byte[0] ^= imm8;
		result = cpu.reg.eax.dword;
		
		resultSize = 8;
	}
	else
		assert(0);
	
	assert(resultSize != 0);
	
	/* clear flags; some will be set after this */
	cpu.reg.eflags.dword &= FLAG_CF | FLAG_PF | FLAG_ZF | FLAG_SF | FLAG_OF;
	
	/* set PF for even number of 1's in the low byte of the result */
	cpu.reg.eflags.dword |= FLAG_PF;
	for (uint8_t i = 0; i < 8; ++i)
		if (result & _BV(i))
			cpu.reg.eflags.dword ^= FLAG_PF;
	
	/* set ZF for zero result */
	if ((resultSize == 8 && (result & 0xff) == 0) ||
		(resultSize == 16 && (result & 0xffff) == 0) ||
		(resultSize == 32 && result == 0))
		cpu.reg.eflags.dword |= FLAG_ZF;
	
	/* set SF for the highest bit in the result */
	if ((result & _BV(resultSize - 1)) != 0)
		cpu.reg.eflags.dword |= FLAG_SF;
}

void instrXCHG(void)
{
	dbgHeader();
	uartWritePSTR("xchg\n");
	
	if (instr[0] >= 0x91 && instr[0] <= 0x97) // xchg ax|eax,reg16|32
	{
		Reg *reg = gpReg[instr[0] - 0x90];
		
		if (prefix & PFX_OPSIZE) // xchg ax,reg16
		{
			uint16_t temp;
			
			cpu.reg.eip.dword += 2;
			
			temp = cpu.reg.eax.word[0];
			cpu.reg.eax.word[0] = reg->word[0];
			reg->word[0] = temp;
		}
		else // xchg eax,reg32
		{
			uint32_t temp;
			
			++cpu.reg.eip.dword;
			
			temp = cpu.reg.eax.dword;
			cpu.reg.eax.dword = reg->dword;
			reg->dword = temp;
		}
	}
	else
		assert(0);
	
	/* XCHG does not affect flags */
}

void instrPUSH(void)
{
	dbgHeader();
	uartWritePSTR("push\n");
	
	if (instr[0] >= 0x50 && instr[0] <= 0x57) // push reg16|32
	{
		/* save the value BEFORE esp is changed so that esp's old value will
		 * get pushed if we are pushing esp */
		Reg reg = *gpReg[instr[0] - 0x50];
		
		if (prefix & PFX_OPSIZE) // push reg16
		{
			cpu.reg.eip.dword += 2;
			
			cpu.reg.esp.dword -= 2;
			memWriteWORD(cpu.reg.esp.dword, reg.word[0]);
		}
		else // push reg32
		{
			++cpu.reg.eip.dword;
			
			cpu.reg.esp.dword -= 4;
			memWriteDWORD(cpu.reg.esp.dword, reg.dword);
		}
	}
	else
		assert(0);
	
	/* PUSH does not affect flags */
}

void instrPOP(void)
{
	dbgHeader();
	uartWritePSTR("pop\n");
	
	if (instr[0] >= 0x58 && instr[0] <= 0x5f) // pop reg16|32
	{
		Reg *reg = gpReg[instr[0] - 0x58];
		
		if (prefix & PFX_OPSIZE) // pop reg16
		{
			cpu.reg.eip.dword += 2;
			
			reg->word[0] = memReadWORD(cpu.reg.esp.dword);
			cpu.reg.esp.dword += 2;
		}
		else // pop reg32
		{
			++cpu.reg.eip.dword;
			
			reg->dword = memReadDWORD(cpu.reg.esp.dword);
			cpu.reg.esp.dword += 4;
		}
	}
	else
		assert(0);
	
	/* POP does not affect flags */
}

void instrNOP(void)
{
	dbgHeader();
	uartWritePSTR("nop\n");
	
	++cpu.reg.eip.dword;
	
	/* NOP does not affect flags */
}

void instrIncReg(void)
{
	dbgHeader();
	uartWritePSTR("inc reg\n");
	
	Reg *reg = gpReg[instr[0] - 0x40];
	
	if (prefix & PFX_OPSIZE) // inc reg16
	{
		cpu.reg.eip.dword += 2;
		
		++(reg->word[0]);
	}
	else // dec reg16
	{
		++cpu.reg.eip.dword;
		
		++(reg->dword);
	}
	
	/* TODO: flags */
}

void instrDecReg(void)
{
	dbgHeader();
	uartWritePSTR("dec reg\n");
	
	Reg *reg = gpReg[instr[0] - 0x48];
	
	if (prefix & PFX_OPSIZE) // inc reg16
	{
		cpu.reg.eip.dword += 2;
		
		--(reg->word[0]);
	}
	else // dec reg16
	{
		++cpu.reg.eip.dword;
		
		--(reg->dword);
	}
	
	
}

#endif
