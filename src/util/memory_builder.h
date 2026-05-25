#ifndef MEMORY_BUILDER_H
#define MEMORY_BUILDER_H

#include <vector>
#include <QString>
#include "../common/common_defs.h"

struct MemoryLine
{
    offs_t address;
    QString data;
    int byte_count;
};

class MemoryBuilder
{
public:
    static void build(std::vector<MemoryLine> *lines, offs_t start, offs_t end, uint8_t *memory);
};

#endif MEMORY_BUILDER_H