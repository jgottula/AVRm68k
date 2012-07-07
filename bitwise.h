#ifndef JGOTTULA_BITWISE_H
#define JGOTTULA_BITWISE_H

#include <stdbool.h>
#include <stdint.h>

uint16_t decodeBigEndian16(const uint8_t *bytes);
uint32_t decodeBigEndian32(const uint8_t *bytes);
uint16_t signExtend8to16(uint8_t byte);
uint32_t signExtend8to32(uint8_t byte);
uint32_t signExtend16to32(uint16_t word);

#endif
