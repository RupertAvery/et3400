#include "address_decoder.h"

bool is_pattern_valid(const char *pat)
{
    int bit = 15;

    for (const char *p = pat; *p; p++)
    {
        char c = *p;
        if (c == ' ')
            continue;
        if (c == '1' || c == '0' || c == 'X' || c == 'x')
        {
            if (--bit < -1)
                return false; // too many bits
        }
        else
        {
            return false; // invalid character
        }
    }

    return bit == -1; // exactly 16 bits consumed
}

BitPattern parse_pattern(const char *pat)
{
    BitPattern bp = {0, 0};
    int bit = 15; // MSB first
    uint16_t start = 0;
    uint16_t end = 0;
    bp.pattern[16] = 0;

    for (const char *p = pat; *p && bit >= 0; p++)
    {
        char c = *p;
        if (c == ' ')
            continue;
        if (c == '1')
        {
            bp.mask |= (1u << bit);
            bp.expected |= (1u << bit);
            bp.pattern[15 - bit] = c;

            start |= (1u << bit);
            end |= (1u << bit);

            bit--;
        }
        else if (c == '0')
        {
            bp.mask |= (1u << bit);
            // expected bit stays 0
            bp.pattern[15 - bit] = c;

            bit--;
        }
        else if (c == 'X' || c == 'x')
        {
            // mask bit stays 0 — this position is ignored
            // start bit stays 0
            // end bit set to 1
            end |= (1u << bit);
            bp.pattern[15 - bit] = 'X';

            bit--;
        }
    }

    // compute start and end addresses for this bit pattern
    bp.start = start;
    bp.end = end;

    return bp;
}

bool match_pattern(BitPattern bp, uint16_t input)
{
    return (input & bp.mask) == bp.expected;
}
