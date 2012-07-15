#ifndef JGOTTULA_M68K_INSTR_H
#define JGOTTULA_M68K_INSTR_H

#include <stdbool.h>
#include <stdint.h>

void instrEmu(void);
void instrAddq(void);
void instrAndEorOr(bool or, bool exclusive, bool dataRegDest);
void instrAndiEoriOri(bool or, bool exclusive);
void instrBcc(void);
void instrBra(void);
void instrBsetBtst(bool set, bool immediate);
void instrBsr(void);
void instrClr(void);
void instrDbcc(void);
void instrExg(void);
void instrExt(void);
void instrJmp(void);
void instrJsr(void);
void instrLea(void);
void instrMove(void);
void instrMoveFromCcr(void);
void instrMoveFromSr(void);
void instrMovem(bool regToMem);
void instrMoveq(void);
void instrMoveToCcr(void);
void instrMoveToSr(void);
void instrNeg(bool extend);
void instrNop(void);
void instrNot(void);
void instrPea(void);
void instrRts(void);
void instrScc(void);
void instrSwap(void);
void instrTst(void);

#endif
