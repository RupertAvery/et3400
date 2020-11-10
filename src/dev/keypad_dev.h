#ifndef KEYPAD_DEV_H
#define KEYPAD_DEV_H

#include "memory_map.h"
#include <functional>

class keypad_io : public memory_mapped_device
{
public:
    enum Keys
    {
        Key0,
        Key1,
        Key2,
        Key3,
        Key4,
        Key5,
        Key6,
        Key7,
        Key8,
        Key9,
        KeyA,
        KeyB,
        KeyC,
        KeyD,
        KeyE,
        KeyF,
        KeyReset
    };

    keypad_io();
    //~keypad_io();
    uint8_t read(offs_t addr) override;
    void write(offs_t addr, uint8_t data) override;
    bool is_mapped(offs_t addr) override;
    uint8_t *get_mapped_memory() override;
    offs_t get_start() override;
    offs_t get_end() override;

    void init();
    void press_key(Keys key);
    void release_key(Keys key);
	std::function<void()> on_reset_press;
    const int C006 = 3;
    const int C005 = 2;
    const int C003 = 0;


private:
    uint8_t memory[4];
};

#endif //KEYPAD_DEV_H