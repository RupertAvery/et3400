#ifndef ET3400EMU_H
#define ET3400EMU_H

#include "../util/common.h"
#include "../cpu/m6800.h"
#include "../dev/memory_map.h"
#include "../dev/memory_dev.h"
#include "../dev/display_dev.h"
#include "../dev/keypad_dev.h"
#include "../util/map.h"

#include <functional>

class et3400emu
{

public:
	static const off_t RAM = 0;
	static const off_t MONITOR_ROM = 63;
	static const off_t DISPLAY = 48;
	static const off_t KEYPAD = 48;

	et3400emu(keypad_io *keypad, display_io *display);
	~et3400emu();

	void init();
	void start();
	void stop();
	void reset();

	void halt();
	void step();
	void resume();

	void loadROM(offs_t address, uint8_t *buffer, size_t size);
	void loadRAM(offs_t address, uint8_t *buffer, size_t size);
	void loadMap();
	// uint8_t *get_memory();
	bool get_running();
	int get_cycles();
	CpuStatus get_status();

	void set_clock_rate(int clock_rate);
	int get_clock_rate();
	unsigned long long total_cycles;
	std::function<void()> on_render_frame;

	memory_device *ram;
	memory_device *rom;
	display_io *display;
	keypad_io *keypad;
	memory_map *memory_map;
	std::vector<Map> *maps;

private:
	void render_frame();
	int cycles;
	int clock_rate;
	m6800_cpu_device *device;
	void worker();
	bool running;
	std::thread thread;
};

#endif // ET3400EMU_H