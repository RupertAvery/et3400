#include "display_dev.h"

display_io::display_io()
{
	next = nullptr;
	name = "Display";
	can_disassemble = false;
}

uint8_t display_io::read(offs_t addr)
{
	addr = addr - 0xC100;
	//addr = addr & 0b01110111;
	return displaymem[addr];
};

void display_io::write(offs_t addr, uint8_t data)
{
	addr = addr - 0xC100;
	addr = addr & 0b01110111;
	// write to all addresses that mirror the same data
	displaymem[addr] = data;
	displaymem[addr | 0x08] = data;
	displaymem[addr | 0x80] = data;
	displaymem[addr | 0x88] = data;
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
	return 0xC110;
}

offs_t display_io::get_end()
{
	return 0xC1FF;
}
