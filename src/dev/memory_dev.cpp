#include "memory_dev.h"

memory_device::memory_device(offs_t start, size_t size, bool readonly)
{
    this->readonly = readonly;
    this->start = start;
    this->size = size;
    end = start + size - 1;
    memory = (uint8_t *)malloc(size);
    next = NULL;
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

void memory_device::load(offs_t addr, uint8_t *data, int size)
{
    memcpy(&memory[addr - start], data, size);
}
