#include "file.h"
#include "../util/srec.h"
#include "../util/breakpoint.h"
#include <filesystem>

void File::load_rom(QWidget *parent, et3400emu *emu_ptr)
{
	QString fileName = QFileDialog::getOpenFileName(parent,
													"Load File to ROM", "", "BIN Files (*.bin);;All files (*)");
	if (fileName == nullptr)
		return;

	// calculate size of rom from file size
	size_t size = std::filesystem::file_size(fileName.toStdString());
	// calculate address from FFFF - size
	uint16_t address = 0xFFFF - size + 1;

	emu_ptr->stop();

	emu_ptr->loadROM(fileName, address, size);

	emu_ptr->reset();
	emu_ptr->start();
}

void File::load_ram(QWidget *parent, et3400emu *emu_ptr)
{
	QString fileName = QFileDialog::getOpenFileName(parent,
													"Load File to RAM", "", "SREC Files (*.obj *.s19);;All files (*)");
	if (fileName == nullptr)
		return;

	// pause emulation to avoid overwriting memory while executing
	emu_ptr->stop();

	File::load(emu_ptr, fileName);

	emu_ptr->breakpoints->clearRamBreakpoints();
	//emu_ptr->labels->clearRamLabels();

	// reset and resume emulation
	emu_ptr->reset();
	emu_ptr->start();
}

void File::load(et3400emu *emu_ptr, QString path)
{
	// load S19 blocks
	std::vector<srec_block> *blocks = new std::vector<srec_block>;

	if (SrecReader::Read(path, blocks))
	{
		// write blocks to memory
		for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
		{
			emu_ptr->loadRAM(it->address, it->data, it->bytecount);
		}
	}

	// clean up
	for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
	{
		free(it->data);
	}

	delete blocks;
}

void File::save_ram(QWidget *parent, et3400emu *emu_ptr)
{
	QString fileName = QFileDialog::getSaveFileName(parent,
													"Save RAM Contents", "", "SREC Files (*.s19)");

	if (fileName == nullptr)
		return;

	std::vector<srec_block> *blocks = new std::vector<srec_block>;

	// pause emulation to avoid reading changing memory while executing
	emu_ptr->stop();

	memory_mapped_device *ram = emu_ptr->memory_map->get_block_device(0x0000);

	uint8_t *memory = ram->get_mapped_memory();

	static const int BLOCK_SIZE = 16;
	int bytecount = 16;
	int i = 0;
	int address = 0;

	while (address <= ram->get_end())
	{
		// check if this block of data is all zeros
		int zeros = 0;
		for (int i = 0; i < 16 && address + i <= ram->get_end(); i++)
		{
#ifdef _DEBUG
			zeros += memory[address + i] == 0xCD ? 1 : 0;
#else
			zeros += memory[address + i] == 0x00 ? 1 : 0;
#endif // DEBUG
		}

		if (zeros < BLOCK_SIZE)
		{
			blocks->push_back(srec_block{bytecount, address, &memory[address]});
		}

		address += BLOCK_SIZE;
	}

	// save S19 blocks
	SrecReader::Write(fileName, blocks);

	delete blocks;

	// resume emulation
	emu_ptr->start();
}