#include "et3400.h"

et3400emu::et3400emu()
{
    this->device = new m6800_cpu_device;
    this->running = false;
}

et3400emu::~et3400emu()
{
    free(this->device);
}

void et3400emu::loadROM(offs_t address, uint8_t *buffer, size_t size)
{
    device->write_bytes(address, (uint8_t *)buffer, size);
}

void et3400emu::loadRAM(offs_t address, uint8_t *buffer, size_t size)
{
    device->write_bytes(address, (uint8_t *)buffer, size);
}

bool et3400emu::get_running()
{
    return running;
}

void et3400emu::stop()
{
    LOG("Stopping emulator thread");
    running = false;
    thread.join();
}

uint8_t *et3400emu::get_memory()
{
    return device->memory;
}

void et3400emu::init()
{
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

void et3400emu::worker()
{
    device->device_start();
    device->device_reset();

    while (this->running)
    {
        device->m_icount = 1667;
        device->pre_execute_run();
        device->execute_run();
        sleep(1667);
    }

    LOG("emulator thread exited");
}

void et3400emu::press_key(Keys key)
{
    switch (key)
    {
    // pull appropriate bit at mem location LOW
    case Key0:
        device->memory[0xC006] &= 0xDF;
        break;
    case Key1: // 1, ACCA
        device->memory[0xC006] &= 0xEF;
        break;
    case Key2: // 2
        device->memory[0xC005] &= 0xEF;
        break;
    case Key3: // 3
        device->memory[0xC003] &= 0xEF;
        break;
    case Key4: // 4, INDEX
        device->memory[0xC006] &= 0xF7;
        break;
    case Key5: // 5, CC
        device->memory[0xC005] &= 0xF7;
        break;
    case Key6: // 6
        device->memory[0xC003] &= 0xF7;
        break;
    case Key7: // 7, RTI;
        device->memory[0xC006] &= 0xFB;
        break;
    case Key8: // 8
        device->memory[0xC005] &= 0xFB;
        break;
    case Key9: // 9
        device->memory[0xC003] &= 0xFB;
        break;
    case KeyA: // A, Auto
        device->memory[0xC006] &= 0xFD;
        break;
    case KeyB: // B
        device->memory[0xC005] &= 0xFD;
        break;
    case KeyC: // C
        device->memory[0xC003] &= 0xFD;
        break;
    case KeyD: // D, Do
        device->memory[0xC006] &= 0xFE;
        break;
    case KeyE: // E, Exam
        device->memory[0xC005] &= 0xFE;
        break;
    case KeyF: // F
        device->memory[0xC003] &= 0xFE;
        break;
    case KeyReset: // RESET
        device->reset_line = 0;
        break;
    }
}

void et3400emu::release_key(Keys key)
{
    // just pull everything high.
    // we're not monitoring multiple presses anyway
    device->memory[0xC003] = 0xFF;
    device->memory[0xC005] = 0xFF;
    device->memory[0xC006] = 0xFF;
    switch (key)
    {
    case KeyReset: // RESET
                   //device->reset_line = 0;
        break;
    }
}
