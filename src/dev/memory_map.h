#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <sys/types.h>
#include <map>
#include <vector>
#include "memory_mapped_device.h"
#include "../util/breakpoint_manager.h"

class MemoryMapManager
{
public:
    MemoryMapManager();
    ~MemoryMapManager();

    BreakpointManager *breakpoints = nullptr;

    void map(memory_mapped_device *device);
    void unmap(memory_mapped_device *device);
    memory_mapped_device *get_block_device(off_t address);
    memory_mapped_device *try_get_block_device(std::string device_name);
    std::vector<memory_mapped_device *> get_block_devices();

    uint8_t read(offs_t addr);
    void write(offs_t addr, uint8_t data);

private:
    const int BLOCK_SIZE = 1024;
    mapped_memory_block blocks[64];
};

#endif // MEMORY_MAP_H