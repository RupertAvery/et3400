#include "stdlib.h"
#include "custom_dev.h"
#include "string.h"
#include <string>
#include "../util/log.h"

custom_device::custom_device(std::string name, BitPattern bit_pattern, bool readonly)
{
    this->readonly = readonly;
    set_bit_pattern(bit_pattern);
    this->name = name;

    next = NULL;
    can_disassemble = true;
};

custom_device::~custom_device()
{
    free(memory);
}

uint8_t custom_device::read(offs_t addr)
{
    if (read_hook != nullptr)
    {
        read_hook(addr);
    }
    return memory[addr - start];
};

void custom_device::write(offs_t addr, uint8_t data)
{
    if (!readonly)
    {
        memory[addr - start] = data;
        if (write_hook != nullptr)
        {
            write_hook(addr, data);
        }
    }
};

void custom_device::write_block(offs_t addr, uint8_t *data, size_t size)
{
    if (!readonly)
    {
        memcpy(&memory[addr - start], data, size);
    }
};

bool custom_device::is_mapped(offs_t addr)
{
    return addr >= start && addr <= end;
}

uint8_t *custom_device::get_mapped_memory()
{
    return memory;
};

offs_t custom_device::get_start()
{
    return start;
};

offs_t custom_device::get_end()
{
    return end;
};

offs_t custom_device::get_size()
{
    return size;
};

char *custom_device::get_pattern()
{
    return bit_pattern.pattern;
}
