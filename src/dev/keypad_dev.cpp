#include "keypad_dev.h"

keypad_io::keypad_io()
{
    next = NULL;
}

void keypad_io::init()
{
    C006 = 0xFF;
    C005 = 0xFF;
    C003 = 0xFF;
}

void keypad_io::press_key(Keys key)
{
    switch (key)
    {
    // pull appropriate bit at mem location LOW
    case Key0:
        C006 &= 0xDF;
        break;
    case Key1: // 1, ACCA
        C006 &= 0xEF;
        break;
    case Key2: // 2
        C005 &= 0xEF;
        break;
    case Key3: // 3
        C003 &= 0xEF;
        break;
    case Key4: // 4, INDEX
        C006 &= 0xF7;
        break;
    case Key5: // 5, CC
        C005 &= 0xF7;
        break;
    case Key6: // 6
        C003 &= 0xF7;
        break;
    case Key7: // 7, RTI;
        C006 &= 0xFB;
        break;
    case Key8: // 8
        C005 &= 0xFB;
        break;
    case Key9: // 9
        C003 &= 0xFB;
        break;
    case KeyA: // A, Auto
        C006 &= 0xFD;
        break;
    case KeyB: // B
        C005 &= 0xFD;
        break;
    case KeyC: // C
        C003 &= 0xFD;
        break;
    case KeyD: // D, Do
        C006 &= 0xFE;
        break;
    case KeyE: // E, Exam
        C005 &= 0xFE;
        break;
    case KeyF: // F
        C003 &= 0xFE;
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
    C003 = 0xFF;
    C005 = 0xFF;
    C006 = 0xFF;
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
    return addr == 0xC003 ? C003 : addr == 0xC005 ? C005 : addr == 0xC006 ? C006 : 0;
};

void keypad_io::write(offs_t addr, uint8_t data){

};

bool keypad_io::is_mapped(offs_t addr)
{
    return addr >= 0xC003 && addr <= 0xC006;
}
