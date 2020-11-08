#include "et3400.h"
#include <iostream>

et3400emu::et3400emu(keypad_io *keypad_dev, display_io *display_dev)
{
    clock_rate = 100;
    memory_map = new ::memory_map;
    device = new m6800_cpu_device(memory_map);
    running = false;
    cycles = 0;
    total_cycles = 0;
    ram = new memory_device(0x0000, 0x0400, false);
    rom = new memory_device(0xFC00, 0x0400, true);

    this->keypad = keypad_dev;
    this->display = display_dev;

    memory_map->map(et3400emu::RAM, ram);
    memory_map->map(et3400emu::MONITOR_ROM, rom);
    memory_map->map(et3400emu::KEYPAD, keypad);
    memory_map->map(et3400emu::DISPLAY, display);
}

et3400emu::~et3400emu()
{
    stop();
    delete ram;
    delete rom;
    delete memory_map;
    delete device;
}

void et3400emu::loadROM(offs_t address, uint8_t *buffer, size_t size)
{
    rom->load(address, buffer, size);
}

void et3400emu::loadRAM(offs_t address, uint8_t *buffer, size_t size)
{
    ram->load(address, buffer, size);
}

bool et3400emu::get_running()
{
    return running;
}

void et3400emu::stop()
{
    if (running)
    {
        LOG("Stopping emulator thread");
        running = false;
        thread.join();
    }
}

void et3400emu::halt()
{
    if (running)
    {
        LOG("Stopping emulator thread");
        running = false;
        thread.join();
    }
}

void et3400emu::step()
{
    if (running)
    {
        device->execute_step();
    }
}

void et3400emu::resume()
{
    LOG("Starting emulator thread");
    running = true;
    thread = std::thread(&et3400emu::worker, this);
    //thread.detach();
}

// uint8_t *et3400emu::get_memory()
// {
//     return device->memory;
// }

void et3400emu::init()
{
    total_cycles = 0;

    // clear memory
    //memset(device->memory, 0, 0xFC00);

    // pull keyboard lines high
    keypad->init();

    device->device_start();
    device->device_reset();
}

void et3400emu::start()
{
    LOG("Starting emulator thread");
    running = true;
    thread = std::thread(&et3400emu::worker, this);
    //thread.detach();
}

void et3400emu::reset()
{
    device->reset_line = 0;
}

int et3400emu::get_cycles()
{
    return device->m_icount;
}

void et3400emu::set_clock_rate(int new_clock_rate)
{
    clock_rate = new_clock_rate;
}

int et3400emu::get_clock_rate()
{
    return clock_rate;
}

void et3400emu::worker()
{
    device->device_start();
    device->device_reset();

    const int sleep_ns = 13667;
    const int base_cycles = 16667;
    const float hundred_percent = 100;
    int ctr = 60;

    while (this->running)
    {
        int cycles_per_frame = (int)(base_cycles * (float)clock_rate / hundred_percent);
        device->m_icount = cycles_per_frame;
        device->pre_execute_run();
        device->execute_run();
        sleep(sleep_ns);
        total_cycles += cycles_per_frame - device->m_icount;
        render_frame();
        //std::cout << std::hex << device->m_pc.d << std::endl;
    }

    LOG("emulator thread exited");
}

void et3400emu::render_frame()
{
    on_render_frame();
}
