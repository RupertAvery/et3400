#include "file.h"
#include "../util/srec.h"
#include "../util/hex.h"
#include "../util/breakpoint.h"
#include <filesystem>
#include <algorithm>
#include "../common/util.h"
#include "../util/log.h"

QString File::error;

void File::load_rom_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings)
{
	// LoadDialog loadDialog;

	// loadDialog.setSettings(settings);

	// QDialog::DialogCode result = (QDialog::DialogCode)loadDialog.exec();

	// if (result == QDialog::DialogCode::Accepted)
	// {
	//	settings = loadDialog.getSettings();

	QString fileName = QFileDialog::getOpenFileName(parent,
													"Load File to ROM", "", "BIN Files (*.bin);;Motorola S-record files (*.s19 *.obj);;Intel HEX files (*.hex);;All files (*)");
	if (fileName == nullptr)
		return;

	emu_ptr->stop();

	bool success = false;

	load_memory(fileName, "Monitor ROM", emu_ptr, settings.start, success);

	if (!success)
	{
		QMessageBox::critical(parent, "Error loading ROM", error);
	}
	else
	{
		emu_ptr->reset();
	}

	emu_ptr->start();
	//}
}
/*
Loads a file into the specified location
*/
size_t File::load_memory(QString path, QString device_name, et3400emu *emu_ptr, uint16_t address, bool &success)
{
	size_t size = 0;
	error = "";

	memory_mapped_device *device = emu_ptr->memory_map->try_get_block_device(device_name.toStdString());

	if (device)
	{
		offs_t start = device->get_start();
		offs_t end = device->get_end();

		if (is_srec(path))
		{
			LOG_DEBUG << "Loading SREC file";

			std::vector<data_block> *blocks = new std::vector<data_block>;

			success = SrecFile::Read(path, blocks);

			if (success)
			{
				for (std::vector<data_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
				{
					if (start > it->address || end < it->address)
					{
						error = "Failed to load \"" + path + "\".\n";
						error += "Address was out of the target range (" + toHex(start) + "-" + toHex(end) + ")";
						success = false;
						return size;
					}
				}

				for (std::vector<data_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
				{
					device->load(it->address, it->data, it->length);
				}

				free_blocks(blocks);
			}
			else
			{
				error = "Failed to load \"" + path + "\".\n";
				error += SrecFile::error;
				return 0;
			}
		}
		else if (is_hex(path))
		{
			LOG_DEBUG << "Loading HEX file";

			std::vector<data_block> *blocks = new std::vector<data_block>;

			success = HexFile::Read(path, blocks);

			if (success)
			{
				for (std::vector<data_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
				{
					if (start > it->address || end < it->address)
					{
						error = "Failed to load \"" + path + "\".\n";
						error += "Address was out of the target range (" + toHex(start) + "-" + toHex(end) + ")";
						success = false;
						return size;
					}
				}

				for (std::vector<data_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
				{
					device->load(it->address, it->data, it->length);
				}

				free_blocks(blocks);
			}
			else
			{
				error = "Failed to load \"" + path + "\".\n";
				error += HexFile::error;
				return 0;
			}
		}
		else if (is_bin(path))
		{
			LOG_DEBUG << "Loading BIN file";

			char *buffer = load_bin(path, size, success);

			if (success)
			{
				if (size != device->get_size())
				{
					success = false;
					error = "Failed to load \"" + path + "\".\n";
					error += "Expected size: " + QString::number(device->get_size()) + ", found: " + QString::number(size);
					return size;
				}
				else
				{
					device->load(address, (uint8_t *)buffer, size);
				}
				free(buffer);
			}
			else
			{
				error = "Failed to load \"" + path + "\".\n";
				return 0;
			}
		}
		else
		{
			success = false;
			error = "Failed to load \"" + path + "\".\n";
			error += "Unsupported file type";
			return size;
		}

		if (!success)
		{
			error = "Failed to load \"" + path + "\".";
		}
	}
	else
	{
		success = false;
		error = "Failed to acquire device \"" + device_name + "\".";
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
													"Load File to RAM", "", "Motorola S-record files (*.s19 *.obj);;Intel HEX files (*.hex);;All files (*)");
	if (fileName == nullptr)
		return;

	// pause emulation to avoid overwriting memory while executing
	emu_ptr->stop();

	bool success;

	load_memory(fileName, "RAM", emu_ptr, settings.start, success);

	if (!success)
	{
		QMessageBox::critical(parent, "Error loading RAM", error);
		return;
	}
	else
	{
		emu_ptr->breakpoints->clearRamBreakpoints();
		// emu_ptr->labels->clearRamLabels();

		// reset and resume emulation
		emu_ptr->reset();
	}

	emu_ptr->start();
	//}
}

void File::save_ram_dialog(QWidget *parent, et3400emu *emu_ptr, SaveSettings &settings)
{
	SaveDialog saveDialog;

	saveDialog.setSettings(settings);

	QDialog::DialogCode result = (QDialog::DialogCode)saveDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		settings = saveDialog.getSettings();

		QString fileName = QFileDialog::getSaveFileName(parent,
														"Save RAM Contents", "", "Motorola S-record files (*.s19 *.obj);;Intel HEX files (*.hex);;All files (*)");

		if (fileName == nullptr)
			return;

		std::vector<data_block> *blocks = new std::vector<data_block>;

		// pause emulation to avoid reading changing memory while executing
		emu_ptr->stop();

		memory_mapped_device *ram = emu_ptr->memory_map->try_get_block_device("RAM");

		uint8_t *memory = ram->get_mapped_memory();

		static const uint16_t BLOCK_SIZE = 16;

		uint16_t address = settings.start;
		uint16_t endAddress = std::min(((uint16_t)ram->get_end()), (uint16_t)settings.end);

		const uint16_t monitor_ram_start = 0x00C5;
		const uint16_t monitor_ram_end = 0x00FF;

		while (address < endAddress)
		{
			if (address <= monitor_ram_end)
			{
				if (address + BLOCK_SIZE >= monitor_ram_start)
				{
					// Save block up to the start of Monitor RAM
					uint16_t bytecount = std::min((uint16_t)(monitor_ram_start - address), BLOCK_SIZE);
					blocks->push_back(data_block{bytecount, address, &memory[address]});
					// Move past end of Monitor RAM
					address = monitor_ram_end + 1;
					continue;
				}
			}

			uint16_t bytecount = std::min((uint16_t)(endAddress - address + 1), BLOCK_SIZE);
			blocks->push_back(data_block{bytecount, address, &memory[address]});

			address += bytecount;
		}

		if (is_srec(fileName))
		{
			// save S19 blocks
			SrecFile::Write(fileName, settings.header, blocks, 0);
		}
		else if (is_hex(fileName))
		{
			// save S19 blocks
			HexFile::Write(fileName, blocks);
		}
		else
		{
			// save S19 blocks
			SrecFile::Write(fileName, settings.header, blocks, 0);
		}

		delete blocks;

		// resume emulation
		emu_ptr->start();
	}
}