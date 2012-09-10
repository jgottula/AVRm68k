/* AVRm68k [emulator subproject]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* uart access and utility functions */

#ifndef AVRM68K_EMULATOR_UART_H
#define AVRM68K_EMULATOR_UART_H


bool uartAvail(void);
uint8_t uartRead(void);
void uartWriteDec16(uint16_t word);
void uartWriteHex8(uint8_t byte, bool upper);
void uartWriteHex16(uint16_t word, bool upper);
void uartWriteHexTop12(uint32_t wordAndAHalf, bool upper);
void uartWriteHexBottom12(uint32_t wordAndAHalf, bool upper);
void uartWriteHex24(uint32_t wordAndAHalf, bool upper);
void uartWriteHex32(uint32_t dword, bool upper);
void uartWriteHex256(const uint32_t arr[8], bool upper);
void uartWriteBin4(uint8_t nibble);
void uartWriteBin8(uint8_t byte);
void uartWriteBool(bool truth);
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
void uartWriteHex4(uint8_t nibble, bool upper);


#define uartWritePSTR(_str) (uartWritePStr(PSTR(_str)))


#endif
