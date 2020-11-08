#include "display_dev.h"

display_io::display_io() {
	next = NULL;
}

uint8_t display_io::read(offs_t addr)
{
	return displaymem[addr - 0xC110];
};

void display_io::write(offs_t addr, uint8_t data)
{
	displaymem[addr - 0xC110] = data;
};

bool display_io::is_mapped(offs_t addr)
{
	return addr >= 0xC110 && addr <= 0xC16F;
}
