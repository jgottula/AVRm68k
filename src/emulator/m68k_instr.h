/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* m68k emulated instruction implementations */

#ifndef AVRM68K_EMULATOR_M68K_INSTR_H
#define AVRM68K_EMULATOR_M68K_INSTR_H


void instrEmu(void);
void instrAddq(void);
void instrAndEorOr(bool or, bool exclusive, bool dataRegDest);
void instrAndiEoriOri(bool or, bool exclusive);
void instrAslAsrMem(bool left);
void instrAslAsrReg(bool left, bool immediate);
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
void instrLink(uint8_t size);
void instrMove(void);
void instrMoveFromCcr(void);
void instrMoveFromSr(void);
void instrMovem(bool regToMem);
void instrMoveq(void);
void instrMoveToCcr(void);
void instrMoveToSr(void);
void instrMoveUsp(bool toUSP);
void instrNeg(bool extend);
void instrNop(void);
void instrNot(void);
void instrPea(void);
void instrRts(void);
void instrScc(void);
void instrSwap(void);
void instrTst(void);
void instrUnlk(void);


#endif
