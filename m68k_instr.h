#ifndef JGOTTULA_M68K_INSTR_H
#define JGOTTULA_M68K_INSTR_H

#include <stdbool.h>
#include <stdint.h>

bool instrEmu(void);
void instrAddq(void);
void instrBcc(void);
void instrBra(void);
void instrBsr(void);
void instrClr(void);
void instrEorOrAnd(bool or, bool exclusive, bool dataRegDest);
void instrEoriOriAndi(bool or, bool exclusive);
void instrExg(void);
void instrExt(void);
void instrJmp(void);
void instrJsr(void);
void instrLea(void);
void instrMove(void);
void instrMoveFromCcr(void);
void instrMoveFromSr(void);
void instrMoveq(void);
void instrMoveToCcr(void);
void instrNop(void);
void instrNot(void);
void instrPea(void);
void instrRts(void);
void instrScc(void);
void instrSwap(void);
void instrTst(void);

#endif
