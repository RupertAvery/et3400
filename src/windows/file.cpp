#include "file.h"
#include "../util/srec.h"
#include "../util/breakpoint.h"
#include <filesystem>
#include <algorithm>
#include "../common/util.h"
#include "../util/log.h"

void File::load_rom_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings)
{
	// LoadDialog loadDialog;

	// loadDialog.setSettings(settings);

	// QDialog::DialogCode result = (QDialog::DialogCode)loadDialog.exec();

	// if (result == QDialog::DialogCode::Accepted)
	// {
	//	settings = loadDialog.getSettings();

	QString fileName = QFileDialog::getOpenFileName(parent,
													"Load File to ROM", "", "BIN Files (*.bin);;SREC Files (*.obj, *.s19);;All files (*)");
	if (fileName == nullptr)
		return;

	emu_ptr->stop();

	load_memory(fileName, "Monitor ROM", emu_ptr, settings.start);

	emu_ptr->reset();
	emu_ptr->start();
	//}
}
/*
Loads a file into the specified location
*/
size_t File::load_memory(QString path, QString device_name, et3400emu *emu_ptr, uint16_t address)
{
	size_t size = 0;
	bool success;

	memory_mapped_device *device = emu_ptr->memory_map->try_get_block_device(device_name.toStdString());

	if (device)
	{
		if (is_srec(path))
		{
			std::vector<srec_block> *blocks = new std::vector<srec_block>;

			success = SrecFile::Read(path, blocks);

			if (success)
			{
				for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
				{
					device->load(it->address, it->data, it->length);
				}
				SrecFile::Free(blocks);
			}
		}
		else
		{
			char *buffer = load_bin(path, size, success);

			if (success)
			{
				device->load(address, (uint8_t *)buffer, size);

				free(buffer);
			}
		}
	}

	return size;
}

void File::load_ram_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings)
{
	// LoadDialog loadDialog;

	// loadDialog.setSettings(settings);

	// QDialog::DialogCode result = (QDialog::DialogCode)loadDialog.exec();

	// if (result == QDialog::DialogCode::Accepted)
	// {
	//	settings = loadDialog.getSettings();

	QString fileName = QFileDialog::getOpenFileName(parent,
													"Load File to RAM", "", "SREC Files (*.obj *.s19);;All files (*)");
	if (fileName == nullptr)
		return;

	// pause emulation to avoid overwriting memory while executing
	emu_ptr->stop();

	load_memory(fileName, "RAM", emu_ptr, settings.start);

	emu_ptr->breakpoints->clearRamBreakpoints();
	// emu_ptr->labels->clearRamLabels();

	// reset and resume emulation
	emu_ptr->reset();
	emu_ptr->start();
	//}
}

void File::save_ram_dialog(QWidget *parent, et3400emu *emu_ptr, SaveSettings &settings)
{
	// SaveDialog saveDialog;

	// saveDialog.setSettings(settings);

	// QDialog::DialogCode result = (QDialog::DialogCode)saveDialog.exec();

	// if (result == QDialog::DialogCode::Accepted)
	// {
	// 	settings = saveDialog.getSettings();

	QString fileName = QFileDialog::getSaveFileName(parent,
													"Save RAM Contents", "", "SREC Files (*.s19)");

	if (fileName == nullptr)
		return;

	std::vector<srec_block> *blocks = new std::vector<srec_block>;

	// pause emulation to avoid reading changing memory while executing
	emu_ptr->stop();

	memory_mapped_device *ram = emu_ptr->memory_map->get_block_device(0x0000);

	uint8_t *memory = ram->get_mapped_memory();

	static const uint16_t BLOCK_SIZE = 16;

	uint16_t address = settings.start;
	uint16_t endAddress = std::min(((uint16_t)ram->get_end()), (uint16_t)settings.end);

	while (address < endAddress)
	{
		uint16_t bytecount = std::min((uint16_t)(endAddress - address + 1), BLOCK_SIZE);
		blocks->push_back(srec_block{bytecount, address, &memory[address]});
		address += bytecount;
	}

	// save S19 blocks
	SrecFile::Write(fileName, settings.header, blocks, 0);

	delete blocks;

	// resume emulation
	emu_ptr->start();
	//}
}