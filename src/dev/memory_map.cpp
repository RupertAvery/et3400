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

bool MemoryMapManager::has_collision(offs_t start, offs_t end)
{
    int block_start = start / BLOCK_SIZE;
    int block_end = end / BLOCK_SIZE;

    for (int block = block_start; block <= block_end; block++)
    {
        memory_mapped_device *current_device = blocks[block].device;

        while (current_device != NULL)
        {
            if (start <= current_device->get_end() && end >= current_device->get_start())
            {
                return true;
            }
            current_device = current_device->next;
        }
    }

    return false;
}

void MemoryMapManager::map(memory_mapped_device *device)
{
    int block_start = device->get_start() / BLOCK_SIZE;
    int block_end = device->get_end() / BLOCK_SIZE;

    LOG_DEBUG << "Trying to map device " << QString::fromStdString(device->name) << " into block " << block_start;

    if (blocks[block_start].device == NULL)
    {
        LOG_DEBUG << "Block " << block_start << " is unused";
        LOG_DEBUG << "Mapped device " << QString::fromStdString(device->name) << " to block " << block_start;
        blocks[block_start].device = device;
    }
    else
    {
        LOG_DEBUG << "Block " << block_start << " is used, attempting to chain...";
        // get the currently mapped device
        memory_mapped_device *current_device = blocks[block_start].device;

        // walk the chain until the next free slot is found
        while (current_device->next != NULL)
        {
            current_device = current_device->next;
        }
        current_device->next = device;

        LOG_DEBUG << "Chained device " << QString::fromStdString(device->name) << " to device " << QString::fromStdString(current_device->name) << " in block " << block_start;
    }

    // // Iterate over each block
    // for (int block = block_start; block <= block_end; block++)
    // {
    //     // if block is free, assign the device to the slot
    //     if (blocks[block].device == NULL)
    //     {
    //         LOG_DEBUG << "Block " << block << " is unused";
    //         LOG_DEBUG << "Mapped device " << QString::fromStdString(device->name) << " to block " << block;
    //         blocks[block].device = device;
    //         break;
    //     }
    //     else
    //     {
    //         LOG_DEBUG << "Block " << block << " is used, attempting to chain...";
    //         // get the currently mapped device
    //         memory_mapped_device *current_device = blocks[block].device;

    //         // walk the chain until the next free slot is found
    //         while (current_device->next != NULL)
    //         {
    //             current_device = current_device->next;
    //         }
    //         current_device->next = device;

    //         LOG_DEBUG << "Chained device " << QString::fromStdString(device->name) << " to device " << QString::fromStdString(current_device->name) << " in block " << block;
    //     }
    // }
}

void MemoryMapManager::unmap(memory_mapped_device *device)
{
    int block = device->get_start() / BLOCK_SIZE;
    // int block_end = device->get_end() / BLOCK_SIZE;

    LOG_DEBUG << "Trying to unmap device " << QString::fromStdString(device->name) << " from block " << block;

    // If the device is in the slot, free the slot
    if (blocks[block].device == device)
    {
        LOG_DEBUG << "Block " << block << " matches the device";
        LOG_DEBUG << "Unmapped device " << QString::fromStdString(device->name) << " from block " << block;

        blocks[block].device = device->next;
    }
    else
    {
        LOG_DEBUG << "Block " << block << " did not match the device, crawling the chain...";

        // look for the device in the chain
        memory_mapped_device *current_device = blocks[block].device;

        if (current_device == NULL)
        {
            LOG_DEBUG << "No device found!";
            return;
        }

        while (current_device->next != NULL)
        {
            if (current_device->next == device)
            {
                LOG_DEBUG << "Found device " << QString::fromStdString(current_device->name);

                current_device->next = device->next;
                break;
            }

            current_device = current_device->next;
        }
    }

    // the device is no longer part of any chain
    device->next = NULL;
}

uint8_t MemoryMapManager::read(offs_t addr)
{
    memory_mapped_device *device = get_block_device(addr);

    if (device == NULL || ((device->get_flags() & DEVICE_READ) != DEVICE_READ))
    {
        LOG_DEBUG << QString("Read failed at %1 last_write %2").arg(addr, 4, 16).arg(last_write, 4, 16);
        return last_write;
    }

    uint8_t data = device->read(addr);

    return data;
};

void MemoryMapManager::write(offs_t addr, uint8_t data)
{
    memory_mapped_device *device = get_block_device(addr);

    if (device == NULL || ((device->get_flags() & DEVICE_WRITE) != DEVICE_WRITE))
    {
        LOG_DEBUG << QString("Write failed %1").arg(addr, 4, 16);
        return;
    }

    device->write(addr, data);

    last_write = data;
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

std::vector<custom_device *> MemoryMapManager::get_custom_devices()
{
    std::vector<custom_device *> result;

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
            if (!already_added && device->is_custom())
                result.push_back((custom_device *)device);

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
