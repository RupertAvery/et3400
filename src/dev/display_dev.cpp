#include "display_dev.h"
#include "../util/log.h"

display_io::display_io()
{
	next = nullptr;
	name = "Display";
	can_disassemble = false;
}

uint8_t display_io::read(offs_t addr)
{
	// Return floating gates
	return 0xFF;
};

void display_io::write(offs_t addr, uint8_t data)
{
	if (write_hook != nullptr)
	{
		write_hook(addr, data);
	}
	addr = addr - 0xC100;
	displaymem[addr] = data;
};

bool display_io::is_mapped(offs_t addr)
{
	return addr >= 0xC100 && addr <= 0xC1FF;
}

uint8_t *display_io::get_mapped_memory()
{
	return displaymem;
}

offs_t display_io::get_start()
{
	return 0xC100;
}

offs_t display_io::get_end()
{
	return 0xC1FF;
}
