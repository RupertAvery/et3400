#include "et3400.h"

et3400emu::et3400emu(keypad_io *keypad_dev, display_io *display_dev)
{
    clock_rate = 100;
    memory_map = new MemoryMapManager;
    breakpoints = new BreakpointManager;
    device = new m6800_cpu_device(memory_map);
    device->check_breakpoint = [this](uint32_t address){ return check_breakpoint(address); };
    // device->has_breakpoint = [this](off_t address) { return has_breakpoint(address); };
    // device->on_breakpoint = [this] { handle_breakpoint(); };
    running = false;
    cycles = 0;
    last_pc = 0xFFFF;
    total_cycles = 0;
    ram = new memory_device(0x0000, 0x0400, false);
    rom = new memory_device(0xFC00, 0x0400, true);

    this->keypad = keypad_dev;
    this->display = display_dev;

    memory_map->map(et3400emu::RAM, ram);
    memory_map->map(et3400emu::MONITOR_ROM, rom);
    memory_map->map(et3400emu::KEYPAD, keypad);
    memory_map->map(et3400emu::DISPLAY, display);

    maps = new std::vector<Map>;
}

et3400emu::~et3400emu()
{
    stop();
    delete ram;
    delete rom;
    delete memory_map;
    delete breakpoints;
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

void et3400emu::loadMap()
{
    maps->push_back(Map{0xFC06, 0xFC0B, DATA, QString("CPU UP")});
    maps->push_back(Map{0xFF76, 0xFF95, DATA, QString("OP TABLE")});
    maps->push_back(Map{0xFF96, 0xFFA5, DATA, QString("HEX DISPLAY CODE TABLE")});
    maps->push_back(Map{0xFFA6, 0xFFB5, DATA, QString("KEY VALUE TABLE")});
    maps->push_back(Map{0xFFB6, 0xFFF7, DATA, QString("COMMAND HANDLER ENTRY POINT TABLE")});
    maps->push_back(Map{0xFFF8, 0xFFFF, DATA, QString("INTERRUPT VECTORS")});
}

bool et3400emu::get_running()
{
    return running;
}

CpuStatus et3400emu::get_status()
{
    return device->get_status();
}

void et3400emu::stop()
{
    if (running)
    {
        running = false;
        thread.join();
    }
}

void et3400emu::halt()
{
    running = false;
    thread.join();
}

void et3400emu::step()
{
    if (!running)
    {
        if (device->reset_line == 0)
        {
            device->pre_execute_run();
        }
        else
        {
            device->execute_step();
        }
    }
}

void et3400emu::resume()
{
    if (!running)
    {
        running = true;
        thread = std::thread(&et3400emu::worker, this);
    }
}

void et3400emu::init()
{
    total_cycles = 0;

    // pull keyboard lines high
    keypad->init();

    device->device_start();
    device->device_reset();
}

void et3400emu::start()
{
    running = true;
    thread = std::thread(&et3400emu::worker, this);
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
    }
}

bool et3400emu::check_breakpoint(uint32_t address) {
    if (breakpoints->hasBreakpoint(address) && last_pc != address)
    {
        this->running = false;
        on_breakpoint();
        last_pc = address;
        return true;
    }
    last_pc = 0xFFFF;
    return false;
}

void et3400emu::handle_breakpoint()
{
    stop();
    on_breakpoint();
}

void et3400emu::render_frame()
{
    on_render_frame();
}

memory_mapped_device *et3400emu::get_block_device(offs_t address)
{
    return memory_map->get_block_device(address);
}

void et3400emu::add_breakpoint(offs_t address)
{
    breakpoints->addBreakpoint(address);
}

void et3400emu::remove_breakpoint(offs_t address)
{
    breakpoints->removeBreakpoint(address);
}

bool et3400emu::has_breakpoint(offs_t address)
{
    return breakpoints->hasBreakpoint(address);
}

void et3400emu::add_or_remove_breakpoint(offs_t address)
{
    breakpoints->addOrRemoveBreakpoint(address);
}