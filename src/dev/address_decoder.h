#ifndef ADDRESS_DECODER_H
#define ADDRESS_DECODER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    char pattern[17];
    uint16_t mask;
    uint16_t expected;
    uint16_t start;
    uint16_t end;
} BitPattern;

bool is_pattern_valid(const char *pat);
BitPattern parse_pattern(const char *pat);
bool match_pattern(BitPattern bp, uint16_t input);

#endif // ADDRESS_DECODER_H