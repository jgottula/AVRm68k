#ifndef JGOTTULA_MCU_UART_H
#define JGOTTULA_MCU_UART_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

/* convert \n to \r\n for serial terminals */
#define ADD_CARRIAGE_RETURN

bool uartAvail(void);
uint8_t uartRead(void);
void uartWriteDec16(uint16_t word);
void uartWriteHex4(uint8_t nibble, bool upper);
void uartWriteHex8(uint8_t byte, bool upper);
void uartWriteHex16(uint16_t word, bool upper);
void uartWriteHex24(uint32_t wordAndAHalf, bool upper);
void uartWriteHex32(uint32_t dword, bool upper);
void uartWriteHex256(const uint32_t arr[8], bool upper);
void uartWriteBin4(uint8_t nibble);
void uartWriteBin8(uint8_t byte);
void uartWriteBool(bool truth);
void uartWriteArr(const uint8_t *arr, uint16_t len);
void uartWritePArr(const uint8_t *arr, uint16_t len);
void uartWriteStr(const char *str);
void uartWritePStr(const char *str);
void uartWriteChr(char chr);
void uartWrite(uint8_t byte);
bool uartEnabled(void);
void uartInit(void);

#define uartWritePSTR(_str) (uartWritePStr(PSTR(_str)))

#endif
