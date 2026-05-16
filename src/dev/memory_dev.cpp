#include "stdlib.h"
#include "memory_dev.h"
#include "string.h"
#include <string>

memory_device::memory_device(std::string name, offs_t start, size_t size, bool readonly)
{
    this->readonly = readonly;
    this->start = start;
    this->size = size;
    this->name = name;
    end = start + size - 1;
    memory = (uint8_t *)malloc(size);
    next = NULL;
    can_disassemble = true;
};

memory_device::~memory_device()
{
    free(memory);
}

uint8_t memory_device::read(offs_t addr)
{
    return memory[addr - start];
};

void memory_device::write(offs_t addr, uint8_t data)
{
    if (!readonly)
    {
        memory[addr - start] = data;
    }
};

void memory_device::write_block(offs_t addr, uint8_t *data, size_t size)
{
    if (!readonly)
    {
        memcpy(&memory[addr - start], data, size);
    }
};

bool memory_device::is_mapped(offs_t addr)
{
    return addr >= start && addr <= end;
}

uint8_t *memory_device::get_mapped_memory()
{
    return memory;
};

offs_t memory_device::get_start()
{
    return start;
};

offs_t memory_device::get_end()
{
    return end;
};

offs_t memory_device::get_size()
{
    return size;
};

void memory_device::load(offs_t addr, uint8_t *data, size_t size)
{
    memcpy(&memory[addr - start], data, size);
}
