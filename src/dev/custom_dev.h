#ifndef CUSTOM_DEV_H
#define CUSTOM_DEV_H

#include <cstdlib>
#include "memory_mapped_device.h"
#include "address_decoder.h"

class custom_device : public memory_mapped_device
{
public:
    custom_device(std::string name, BitPattern bit_pattern, bool readonly);
    ~custom_device();
    uint8_t read(offs_t addr) override;
    void write(offs_t addr, uint8_t data) override;
    void write_block(offs_t addr, uint8_t *data, size_t length) override;
    bool is_mapped(offs_t addr) override;
    bool is_custom() override { return true; }
    char *get_pattern();

    void set_bit_pattern(BitPattern bit_pattern)
    {
        this->bit_pattern = bit_pattern;
        this->start = bit_pattern.start;
        this->end = bit_pattern.end;
        this->size = bit_pattern.end - bit_pattern.start + 1;
        this->memory = (uint8_t *)realloc(this->memory, this->size);
    }

    BitPattern get_bit_pattern() { return bit_pattern; }

    uint8_t get_flags() override { return readonly ? DEVICE_READ : (DEVICE_READ | DEVICE_WRITE); }
    uint8_t *get_mapped_memory() override;
    offs_t get_start() override;
    offs_t get_end() override;
    offs_t get_size() override;

private:
    BitPattern bit_pattern;
    bool readonly;
    offs_t start;
    offs_t end;
    size_t size;
    uint8_t *memory = nullptr;
};

#endif // CUSTOM_DEV_H
