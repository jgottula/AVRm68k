#ifndef JGOTTULA_M68K_INSTR_H
#define JGOTTULA_M68K_INSTR_H

#include <stdbool.h>
#include <stdint.h>

bool instrEmu(void);
void instrNop(void);
void instrExg(void);
void instrMoveFromCcr(void);
void instrMoveq(void);
void instrClr(void);
void instrLea(void);

#endif
