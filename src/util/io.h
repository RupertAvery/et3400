#ifndef IO_H
#define IO_H

#include <cstdint>
#include <vector>

struct data_block
{
    uint16_t length;
    uint16_t address;
    uint8_t *data;
};

void free_blocks(std::vector<data_block> *blocks);

#endif // IO_H
