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

	void load_rom(std::string string, offs_t address, uint8_t *buffer, size_t size);
	void load_ram(offs_t address, uint8_t *buffer, size_t size);

	// uint8_t *get_memory();
	bool get_running();
	int get_cycles();
	CpuStatus get_status();
	void add_breakpoint(offs_t address);
	void remove_breakpoint(offs_t address);
	void add_or_remove_breakpoint(offs_t address);
	bool has_breakpoint(offs_t address);
	void handle_breakpoint();
	void set_pc(uint16_t pc);
	void set_sp(uint16_t sp);
	void set_ix(uint16_t ix);
	void set_acca(uint8_t acca);
	void set_accb(uint8_t accb);
	void set_cc(uint8_t cc);

	byte read_byte(offs_t address);
	void set_step_out();

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
	MC6820 *mc6820 = nullptr;
	m6800_cpu_device *device = nullptr;
	std::thread thread;
	int cycles = 0;
	int clock_rate = 0;
	bool is_running = false;
	bool is_step_out = false;

	uint32_t last_pc = 0;
	void worker();
	void render_frame();
	bool check_breakpoint(uint32_t address);
	bool debugger_instruction_hook(uint32_t address);
};

#endif // ET3400EMU_H