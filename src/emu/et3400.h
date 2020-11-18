#ifndef ET3400EMU_H
#define ET3400EMU_H

#include "../util/sleep.h"
#include "../util/disassembly_builder.h"
#include "../util/label_manager.h"
#include "../util/breakpoint_manager.h"
#include "../cpu/m6800.h"
#include "../dev/devices.h"

#include <functional>
#include <mutex>
#include <QFile>
#include <QString>

class et3400emu
{

public:
	et3400emu(keypad_io *keypad, display_io *display);
	~et3400emu();

	void init();
	void start();
	void stop();
	void reset();

	void halt();
	void step();
	void resume();

	void loadROM(QString romPath, offs_t address, size_t size);
	//void loadROM(offs_t address, uint8_t *buffer, size_t size);
	void loadRAM(offs_t address, uint8_t *buffer, size_t size);
	void loadMap(QString mapPath);
	// uint8_t *get_memory();
	bool get_running();
	int get_cycles();
	CpuStatus get_status();
	void add_breakpoint(offs_t address);
	void remove_breakpoint(offs_t address);
	void add_or_remove_breakpoint(offs_t address);
	bool has_breakpoint(offs_t address);
	void handle_breakpoint();

	memory_mapped_device *get_block_device(offs_t address);

	void set_clock_rate(int clock_rate);
	int get_clock_rate();
	unsigned long long total_cycles;
	std::function<void()> on_render_frame;
	std::function<void()> on_breakpoint;

	memory_device *ram;
	display_io *display;
	keypad_io *keypad;

	MemoryMapManager *memory_map;
	BreakpointManager *breakpoints;
	LabelManager *labels;

private:
	MC6820 *mc6820;
	m6800_cpu_device *device;
	std::thread thread;
	int cycles;
	int clock_rate;
	bool running;
	uint32_t last_pc;
	void worker();
	void render_frame();
	bool check_breakpoint(uint32_t address);
};

#endif // ET3400EMU_H