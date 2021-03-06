/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* phase-change memory access code */

#ifndef AVRM68K_EMULATOR_PCM_H
#define AVRM68K_EMULATOR_PCM_H


#define PCM_INSTR_WREN        0b00000110
#define PCM_INSTR_WRDI        0b00000110
#define PCM_INSTR_RDID        0b10011111 // also 0b10011110
#define PCM_INSTR_RDSR        0b00000101
#define PCM_INSTR_WRSR        0b00000001
#define PCM_INSTR_READ        0b00000011
#define PCM_INSTR_FAST_READ   0b00001011
#define PCM_INSTR_DOFR        0b00111011
#define PCM_INSTR_QOFR        0b01101011
#define PCM_INSTR_PP_LEGACY   0b00000010
#define PCM_INSTR_PP_BAWR     0b00100010
#define PCM_INSTR_PP_ALL1     0b11010001
#define PCM_INSTR_DIFP_LEGACY 0b10100010
#define PCM_INSTR_DIFP_BAWR   0b11010011
#define PCM_INSTR_DIFP_ALL1   0b11010101
#define PCM_INSTR_QIFP_LEGACY 0b00110010
#define PCM_INSTR_QIFP_BAWR   0b11010111
#define PCM_INSTR_QIFP_ALL1   0b11011001
#define PCM_INSTR_SEC_ERASE   0b11011000   
#define PCM_INSTR_BULK_ERASE  0b11000111


#define PCM_SR_WIP  _BV(0)
#define PCM_SR_WEL  _BV(1)
#define PCM_SR_BP0  _BV(2)
#define PCM_SR_BP1  _BV(3)
#define PCM_SR_BP2  _BV(4)
#define PCM_SR_TB   _BV(5)
#define PCM_SR_BP3  _BV(6)
#define PCM_SR_SRWD _BV(7)


void pcmReadRange(uint32_t addr, uint16_t num, uint8_t *dest);
uint8_t pcmReadByte(uint32_t addr);
void pcmWriteRange(uint32_t addr, uint16_t num, const uint8_t *src);
void pcmWriteByte(uint32_t addr, uint8_t byte);
void pcmSectorErase(uint32_t addr);
void pcmBulkErase(void);
void pcmInit(void);


#endif
