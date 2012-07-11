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
void instrPea(void);
void instrMove(void);
void instrMoveFromSr(void);
void instrMoveToCcr(void);
void instrNot(void);
void instrJmp(void);
void instrRts(void);
void instrJsr(void);
void instrBra(void);
void instrBsr(void);
void instrScc(void);
void instrTst(void);

#endif
