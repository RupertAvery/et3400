#ifndef DISPLAY_DEV_H
#define DISPLAY_DEV_H

#include "memory_map.h"

class display_io : public memory_mapped_device
{
public:
    display_io();
    // ~display_io();
    uint8_t read(offs_t addr) override;
    void write(offs_t addr, uint8_t data) override;
    bool is_mapped(offs_t addr) override;
    // uint8_t *get_mapped_memory() override;
    // offs_t get_start() override;
    // offs_t get_end() override;

private:
    uint8_t displaymem[96];
};

#endif //DISPLAY_DEV_H