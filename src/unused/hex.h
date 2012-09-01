#ifndef JGOTTULA_HEX_H
#define JGOTTULA_HEX_H

#include <stdint.h>
#include <stdbool.h>

uint8_t hexDecode8(const char hex[2]);
uint16_t hexDecode16(const char hex[4]);
uint16_t hexDecode32(const char hex[8]);

#endif
