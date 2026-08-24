#include "stdlib.h"
#include "io_dev.h"
#include "string.h"
#include <string>
#include "../util/log.h"

io_device::io_device(std::string name, offs_t start, size_t size, bool readonly)
{
    this->readonly = readonly;
    this->start = start;
    this->size = size;
    this->name = name;
    end = start + size - 1;
    memory = (uint8_t *)malloc(size);
    next = NULL;
    can_disassemble = false;
};

io_device::~io_device()
{
    free(memory);
}

uint8_t io_device::read(offs_t addr)
{
    if (read_hook != nullptr)
    {
        return read_hook(addr);
    }

    return memory[addr - start];
};

void io_device::write(offs_t addr, uint8_t data)
{
    if (write_hook != nullptr)
    {
        write_hook(addr, data);
    }

    if (!readonly)
    {
        memory[addr - start] = data;
    }
};

void io_device::write_direct(offs_t index, uint8_t data)
{
    memory[index] = data;
};

void io_device::write_block(offs_t addr, uint8_t *data, size_t size)
{
    if (!readonly)
    {
        memcpy(&memory[addr - start], data, size);
    }
};

bool io_device::is_mapped(offs_t addr)
{
    return addr >= start && addr <= end;
}

uint8_t *io_device::get_mapped_memory()
{
    return memory;
};

offs_t io_device::get_start()
{
    return start;
};

offs_t io_device::get_end()
{
    return end;
};

offs_t io_device::get_size()
{
    return size;
};

void io_device::load(offs_t addr, uint8_t *data, size_t size)
{
    memcpy(&memory[addr - start], data, size);
}
