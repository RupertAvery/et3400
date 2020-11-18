#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <map>
#include "memory_mapped_device.h"

class MemoryMapManager
{
public:
    MemoryMapManager();
    ~MemoryMapManager();

    void map(memory_mapped_device *device);
    memory_mapped_device *get_block_device(off_t address);

    uint8_t read(offs_t addr);
    void write(offs_t addr, uint8_t data);

private:
    const int BLOCK_SIZE = 1024;
    mapped_memory_block blocks[64];
};

#endif // MEMORY_MAP_H