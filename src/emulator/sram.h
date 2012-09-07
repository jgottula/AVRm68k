#ifndef JGOTTULA_EMULATOR_SRAM_H
#define JGOTTULA_EMULATOR_SRAM_H

#include <stdbool.h>
#include <stdint.h>

/* 32 KiB */
#define SRAM_SIZE 0x8000

#define SRAM_SR_MODE_BYTE 0b00000001
#define SRAM_SR_MODE_SEQ  0b01000001
#define SRAM_SR_MODE_PAGE 0b10000001

#define SRAM_INSTR_RDSR 0b00000101
#define SRAM_INSTR_WRSR 0b00000001
#define SRAM_INSTR_READ 0b00000011
#define SRAM_INSTR_WRITE 0b00000010

uint8_t sramReadByte(uint16_t addr);
void sramWriteByte(uint16_t addr, uint8_t byte);
void sramReadSeq(uint16_t addr, uint16_t len, uint8_t *dest);
void sramWriteSeq(uint16_t addr, uint16_t len, const uint8_t *src);

#endif
