#ifndef IO_H
#define IO_H

#include <cstdint>
#include <vector>
#include <QString>

struct data_block
{
    uint16_t length;
    uint16_t address;
    uint8_t *data;
};

bool is_bin(QString filename);

void free_blocks(std::vector<data_block> *blocks);

#endif // IO_H
