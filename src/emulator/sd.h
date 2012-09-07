#ifndef JGOTTULA_EMULATOR_SD_H
#define JGOTTULA_EMULATOR_SD_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

enum {
	SDCMD0 = 0,
	SDCMD_GO_IDLE_STATE = 0,
	
	SDCMD8 = 8,
	SDCMD_SEND_IF_COND = 8,
	
	SDCMD58 = 58,
	SDCMD_READ_OCR = 58
} SDSPICmd;

enum {
	SDR1_PARAM_ERROR = _BV(6),
	SDR1_ADDR_ERROR  = _BV(5),
	SDR1_ERASE_ERROR = _BV(4),
	SDR1_CRC_ERROR   = _BV(3),
	SDR1_ILLEGAL_CMD = _BV(2),
	SDR1_ERASE_RESET = _BV(1),
	SDR1_IDLE        = _BV(0)
} SDR1Bits;

enum {
	SDVOLT_33  = 0b0001,
	SDVOLT_LOW = 0b0010
} SDVoltage;

typedef struct {
	uint8_t resp;
} SDR1;

typedef struct {
	uint8_t resp;
	uint8_t ocr[4];
} SDR3;

typedef struct
{
	uint8_t resp;
	uint8_t data[4];
} SDR7;

typedef union {
	SDR1 r1;
	SDR3 r3;
	SDR7 r7;
} SDRx;

bool sdCardDetect();
void sdInit();

#endif
