#include "memory_map.h"
#include <QDebug>
#include <QString>
#include "../util/log.h"

MemoryMapManager::MemoryMapManager()
{
    for (int i = 0; i < 64; i++)
    {
        blocks[i].device = NULL;
    }
}

MemoryMapManager::~MemoryMapManager()
{
}

void MemoryMapManager::map(memory_mapped_device *device)
{
    int block_start = device->get_start() / BLOCK_SIZE;
    int block_end = device->get_end() / BLOCK_SIZE;

    // Iterate over each block
    for (int block = block_start; block <= block_end; block++)
    {
        // if block is free, assign the device to the slot
        if (blocks[block].device == NULL)
        {
            blocks[block].device = device;
        }
        else
        {
            // get the currently mapped device
            memory_mapped_device *current_device = blocks[block].device;

            // walk the chain until the next free slot is found
            while (current_device->next != NULL)
            {
                current_device = current_device->next;
            }
            current_device->next = device;
        }
    }
}

void MemoryMapManager::unmap(memory_mapped_device *device)
{
    int block_start = device->get_start() / BLOCK_SIZE;
    int block_end = device->get_end() / BLOCK_SIZE;

    // Iterate over each block
    for (int block = block_start; block <= block_end; block++)
    {
        // If the device is in the slot, free the slot
        if (blocks[block].device == device)
        {
            LOG_DEBUG << "";
            blocks[block].device = NULL;
        }
        else
        {
            // look for the device in the chain
            memory_mapped_device *current_device = blocks[block].device;

            while (current_device->next != NULL)
            {
                if (current_device->next == device)
                {
                    current_device->next = device->next;
                    break;
                }

                current_device = current_device->next;
            }
        }
    }
}

uint8_t MemoryMapManager::read(offs_t addr)
{
    memory_mapped_device *device = get_block_device(addr);

    if (device == NULL)
        return 0;

    uint8_t data = device->read(addr);

    if (breakpoints)
        breakpoints->check_read(addr, data);

    return data;
};

void MemoryMapManager::write(offs_t addr, uint8_t data)
{
    memory_mapped_device *device = get_block_device(addr);

    if (device == NULL)
        return;

    device->write(addr, data);

    if (breakpoints)
        breakpoints->check_write(addr, data);
};

std::vector<memory_mapped_device *> MemoryMapManager::get_block_devices()
{
    std::vector<memory_mapped_device *> result;

    for (int block = 0; block < 64; block++)
    {
        memory_mapped_device *device = blocks[block].device;
        while (device != nullptr)
        {
            // deduplicate: a device mapped across multiple blocks appears in each one
            bool already_added = false;
            for (auto *d : result)
            {
                if (d == device)
                {
                    already_added = true;
                    break;
                }
            }
            if (!already_added)
                result.push_back(device);
            device = device->next;
        }
    }

    return result;
}

memory_mapped_device *MemoryMapManager::get_block_device(off_t address)
{
    int block = address / BLOCK_SIZE;

    memory_mapped_device *device = blocks[block].device;

    while (device != NULL && !device->is_mapped(address))
    {
        device = device->next;
    }

    return device;
}

memory_mapped_device *MemoryMapManager::try_get_block_device(std::string device_name)
{
    for (int block = 0; block < 64; block++)
    {
        memory_mapped_device *device = blocks[block].device;

        while (device != NULL)
        {
            if (device->name == device_name)
            {
                return device;
            }

            device = device->next;
        }
    }

    return nullptr;
}
