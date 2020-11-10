#include "keypad_dev.h"

keypad_io::keypad_io()
{
    next = NULL;
}

void keypad_io::init()
{
    memory[C006] = 0xFF;
    memory[C005] = 0xFF;
    memory[C003] = 0xFF;
}

void keypad_io::press_key(Keys key)
{
    switch (key)
    {
    // pull appropriate bit at mem location LOW
    case Key0:
        memory[C006] &= 0xDF;
        break;
    case Key1: // 1, ACCA
        memory[C006] &= 0xEF;
        break;
    case Key2: // 2
        memory[C005] &= 0xEF;
        break;
    case Key3: // 3
        memory[C003] &= 0xEF;
        break;
    case Key4: // 4, INDEX
        memory[C006] &= 0xF7;
        break;
    case Key5: // 5, CC
        memory[C005] &= 0xF7;
        break;
    case Key6: // 6
        memory[C003] &= 0xF7;
        break;
    case Key7: // 7, RTI;
        memory[C006] &= 0xFB;
        break;
    case Key8: // 8
        memory[C005] &= 0xFB;
        break;
    case Key9: // 9
        memory[C003] &= 0xFB;
        break;
    case KeyA: // A, Auto
        memory[C006] &= 0xFD;
        break;
    case KeyB: // B
        memory[C005] &= 0xFD;
        break;
    case KeyC: // C
        memory[C003] &= 0xFD;
        break;
    case KeyD: // D, Do
        memory[C006] &= 0xFE;
        break;
    case KeyE: // E, Exam
        memory[C005] &= 0xFE;
        break;
    case KeyF: // F
        memory[C003] &= 0xFE;
        break;
    case KeyReset: // RESET
        on_reset_press();
        break;
    }
}

void keypad_io::release_key(Keys key)
{
    // just pull everything high.
    // we're not monitoring multiple presses anyway
    memory[C003] = 0xFF;
    memory[C005] = 0xFF;
    memory[C006] = 0xFF;
    switch (key)
    {
    case KeyReset: // RESET
                   //device->reset_line = 0;
        break;
    }
}

// void keypad_io::set_emulator(et3400emu *emu)
// {
//     emu_ptr = emu;
// }

uint8_t keypad_io::read(offs_t addr)
{
    return memory[addr - 0xC003];
};

void keypad_io::write(offs_t addr, uint8_t data){

};

bool keypad_io::is_mapped(offs_t addr)
{
    return addr >= 0xC003 && addr <= 0xC006;
}


uint8_t *keypad_io::get_mapped_memory()
{
	return memory;
}

offs_t keypad_io::get_start()
{
	return 0xC003;
}

offs_t keypad_io::get_end()
{
	return 0xC006;
}
