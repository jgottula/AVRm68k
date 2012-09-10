/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* uart access and utility functions */

#ifndef AVRM68K_EMULATOR_UART_H
#define AVRM68K_EMULATOR_UART_H


void uartWriteDec8(uint8_t byte, bool pad);
void uartWriteDec16(uint16_t word, bool pad);
bool uartAvail(void);
bool uartEnabled(void);
void uartInit(void);


/* assembly */
uint8_t uartRead(void);
void uartWrite(uint8_t byte);
void uartWriteChr(char chr);
void uartWriteStr(const char *str);
void uartWritePStr(const char *pstr);
void uartWriteArr(const uint8_t *arr, uint8_t len);
void uartWritePArr(const uint8_t *arr, uint8_t len);
void uartWriteBool(bool truth);
void uartWriteBin4(uint8_t nibble);
void uartWriteBin8(uint8_t byte);
void uartWriteHex4(uint8_t nibble, bool upper);
void uartWriteHex8(uint8_t byte, bool upper);
void uartWriteHex12(uint16_t byteAndAHalf, bool upper);
void uartWriteHex16(uint16_t word, bool upper);
void uartWriteHex24(uint32_t wordAndAHalf, bool upper);
void uartWriteHex24HighHalf(uint32_t wordAndAHalf, bool upper);
void uartWriteHex32(uint32_t dword, bool upper);


#define uartWritePSTR(_str) (uartWritePStr(PSTR(_str)))


#endif
