#ifndef ET3400EMU_H
#define ET3400EMU_H

#include "../util/common.h"
#include "../cpu/m6800.h"

class et3400emu
{

public:
	et3400emu();
	~et3400emu();

	void init();
	void start();
	void stop();
	void reset();

	void loadROM(offs_t address, uint8_t *buffer, size_t size);
	void loadRAM(offs_t address, uint8_t *buffer, size_t size);

	uint8_t *get_memory();
	bool get_running();
	int get_cycles();

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

	void press_key(Keys key);
	void release_key(Keys key);
	void set_clock_rate(int clock_rate);
	int get_clock_rate();
	unsigned long long total_cycles;

private:
	int cycles;
	int clock_rate;
	m6800_cpu_device *device;
	void worker();
	bool running;
	std::thread thread;
};

#endif // ET3400EMU_H