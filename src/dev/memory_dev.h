#ifndef MEMORY_DEV_H
#define MEMORY_DEV_H

#include "memory_map.h"

class memory_device : public memory_mapped_device
{
public:
    memory_device(offs_t start, size_t size, bool readonly);
    ~memory_device();
    uint8_t read(offs_t addr) override;
    void write(offs_t addr, uint8_t data) override;
    bool is_mapped(offs_t addr) override;
    uint8_t *get_mapped_memory() override;
    offs_t get_start() override;
    offs_t get_end() override;

    void load(offs_t addr, uint8_t *data, int size);

private:
    bool readonly;
    offs_t start;
    offs_t end;
    size_t size;
    uint8_t *memory;
};

#endif //MEMORY_DEV_H
