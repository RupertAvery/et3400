#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H
#include "../cpu/defs.h"
#include <map>

/*
    Memory mapped devices
    
    Devices are mapped into blocks of 1KB (1024). This gives 64 blocks
    When the CPU reads or writes to memory, the address is divided by 1024 to get which block
    it needs to access. 
    
    Some devices may share the same block, such as the keypad and the display. In this case we use
    a linked list. Everytime we map a device to a block, we check if the block's top-level device is empty (NULL)
    and if so, the device becomes the first device in the linked list. The next device will be added to the device's 'next' pointer.

    Everytime a read or write occurs, a call to the top-level device's is_mapped(address) is made, and if it returns false, it gets passed to the next 
    device in the chain.
*/



/*
  The base class for a memory-mapped device
*/
class memory_mapped_device
{
public:
    virtual uint8_t read(offs_t addr) { return 0; }
    virtual void write(offs_t addr, uint8_t data) {}
    virtual bool is_mapped(offs_t addr) { return true; }
    virtual uint8_t *get_mapped_memory() { return nullptr; }
    virtual offs_t get_start() { return 0; }
    virtual offs_t get_end() { return 0; }

    memory_mapped_device *next;
};

struct mapped_memory_block
{
    memory_mapped_device *device;
};

class io_device : public memory_mapped_device
{
};

class memory_map
{
public:
    memory_map();
    ~memory_map();

    void map(int block, memory_mapped_device *device);
    memory_mapped_device *get_block_device(off_t address);

    uint8_t read(offs_t addr);
    void write(offs_t addr, uint8_t data);

private:
    mapped_memory_block blocks[64];
};

#endif // MEMORY_MAP_H