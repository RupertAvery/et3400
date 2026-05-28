#include "file.h"
#include "../util/srec.h"
#include "../util/hex.h"
#include "../util/breakpoint.h"
#include <QFileInfo>
#include <filesystem>
#include <algorithm>
#include "../common/util.h"
#include "../util/log.h"
#include "../util/memory_builder.h"

QString File::error;

QString AllROMExtensions = "All supported files (*.s19 *.obj *.hex *.ihx *.bin)";
QString AllRAMExtensions = "All supported files (*.s19 *.obj *.hex *.ihx)";
QString MotorolaSrecExtensions = "Motorola S-record files (*.s19 *.obj)";
QString IntelHexExtensions = "Intel HEX files (*.hex *.ihx)";
QString BinExtensions = "BIN files (*.bin)";
QString LabelFileExtensions = "Label Files (*.lbl)";
QString BreakpointFileExtensions = "Breakpoint Files (*.brk)";
QString TextFileExtensions = "Text Files (*.txt)";
QString AllFiles = "All files (*)";

void File::load_labels_dialog(QWidget *parent, et3400emu *emu_ptr, QString &dir)
{
    QString fileName = QFileDialog::getOpenFileName(parent, "Load Labels", dir, LabelFileExtensions + ";;" + AllFiles);

    if (fileName.isNull())
        return;

    dir = QFileInfo(fileName).absolutePath();

    bool success;
    load_labels(fileName, emu_ptr, success);
}

void File::load_labels(QString fileName, et3400emu *emu_ptr, bool &success)
{
    LOG_DEBUG << "Loading labels from file:" << fileName;
    std::vector<Label> *labels = LabelReader::Read(fileName, success);

    if (success)
    {
        emu_ptr->labels->addLabels(labels);

        delete labels;
    }
}

void File::save_labels_dialog(QWidget *parent, et3400emu *emu_ptr, QString &dir)
{
    QString fileName = QFileDialog::getSaveFileName(parent, "Save Labels", dir, LabelFileExtensions + ";;" + AllFiles);

    if (fileName.isNull())
        return;

    dir = QFileInfo(fileName).absolutePath();

    bool success;
    std::vector<Label> filteredLabels = emu_ptr->labels->getLabels(0x0000, 0x07FF);
    LabelReader::Write(fileName, &filteredLabels, success);
}

void File::save_labels_dialog(QWidget *parent, et3400emu *emu_ptr, uint32_t start, uint32_t end, QString &dir)
{
    QString fileName = QFileDialog::getSaveFileName(parent, "Save Labels", dir, LabelFileExtensions + ";;" + AllFiles);

    if (fileName.isNull())
        return;

    dir = QFileInfo(fileName).absolutePath();

    bool success;
    std::vector<Label> filteredLabels = emu_ptr->labels->getLabels(start, end);
    LabelReader::Write(fileName, &filteredLabels, success);
}

void File::load_breakpoint_dialog(QWidget *parent, et3400emu *emu_ptr, QString &dir)
{
    QString fileName = QFileDialog::getOpenFileName(parent, "Load Breakpoints", dir,
                                                    BreakpointFileExtensions + ";;" + AllFiles);
    if (fileName.isNull())
        return;

    dir = QFileInfo(fileName).absolutePath();

    bool success;
    emu_ptr->stop();
    emu_ptr->breakpoints->loadBreakpoints(fileName, success);
    emu_ptr->start();
}

void File::save_breakpoint_dialog(QWidget *parent, et3400emu *emu_ptr, QString &dir)
{
    QString fileName = QFileDialog::getSaveFileName(parent, "Save Breakpoints", dir,
                                                    BreakpointFileExtensions + ";;" + AllFiles);
    if (fileName.isNull())
        return;

    dir = QFileInfo(fileName).absolutePath();

    bool success;
    emu_ptr->breakpoints->saveBreakpoints(fileName, success);
}

void File::load_rom_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings, QString &dir)
{
    QString extensions = AllROMExtensions + ";;" + MotorolaSrecExtensions + ";;" + IntelHexExtensions + ";;" + BinExtensions + ";;" + AllFiles;

    QString fileName = QFileDialog::getOpenFileName(parent, "Load File to ROM", dir, extensions);
    if (fileName.isNull())
        return;

    dir = QFileInfo(fileName).absolutePath();

    emu_ptr->stop();

    bool success = false;
    load_memory(fileName, "Monitor ROM", emu_ptr, settings.start, success);

    if (!success)
        QMessageBox::critical(parent, "Error loading ROM", error);
    else
        emu_ptr->reset();

    emu_ptr->start();
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

void File::load_ram_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings, QString &dir)
{
    QString extensions = AllRAMExtensions + ";;" + MotorolaSrecExtensions + ";;" + IntelHexExtensions + ";;" + AllFiles;

    QString fileName = QFileDialog::getOpenFileName(parent, "Load File to RAM", dir, extensions);
    if (fileName.isNull())
        return;

    dir = QFileInfo(fileName).absolutePath();

    emu_ptr->stop();

    bool success;
    load_memory(fileName, "RAM", emu_ptr, settings.start, success);

    if (!success)
    {
        QMessageBox::critical(parent, "Error loading RAM", error);
        emu_ptr->start();
        return;
    }

    emu_ptr->breakpoints->clearRamBreakpoints();
    emu_ptr->reset();
    emu_ptr->start();
}

void File::save_ram_dialog(QWidget *parent, et3400emu *emu_ptr, SaveSettings &settings, QString &dir)
{
    SaveDialog saveDialog;

    saveDialog.setSettings(settings);

    QDialog::DialogCode result = (QDialog::DialogCode)saveDialog.exec();

    if (result == QDialog::DialogCode::Accepted)
    {
        settings = saveDialog.getSettings();

        QString extensions = AllRAMExtensions + ";;" + MotorolaSrecExtensions + ";;" + IntelHexExtensions + ";;" + AllFiles;

        QString fileName = QFileDialog::getSaveFileName(parent, "Save RAM Contents", dir, extensions);

        if (fileName.isNull())
            return;

        dir = QFileInfo(fileName).absolutePath();

        std::vector<data_block> *blocks = new std::vector<data_block>;

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
                    uint16_t bytecount = std::min((uint16_t)(monitor_ram_start - address), BLOCK_SIZE);
                    blocks->push_back(data_block{bytecount, address, &memory[address]});
                    address = monitor_ram_end + 1;
                    continue;
                }
            }

            uint16_t bytecount = std::min((uint16_t)(endAddress - address + 1), BLOCK_SIZE);
            blocks->push_back(data_block{bytecount, address, &memory[address]});

            address += bytecount;
        }

        if (is_srec(fileName))
            SrecFile::Write(fileName, settings.header, blocks, 0);
        else if (is_hex(fileName))
            HexFile::Write(fileName, blocks);
        else
            SrecFile::Write(fileName, settings.header, blocks, 0);

        delete blocks;

        emu_ptr->start();
    }
}

void File::save_memory_mapped_devices_dialog(QWidget *parent, et3400emu *emu_ptr, memory_mapped_device *dasm, memory_mapped_device *mem, SnapFileSettings settings)
{
    QString fileName = QFileDialog::getSaveFileName(parent, "Save View As Text", "",
                                                    TextFileExtensions + ";;" + AllFiles);
    if (fileName.isNull())
        return;

    std::vector<DisassemblyLine> *dasm_lines = new std::vector<DisassemblyLine>;
    std::vector<MemoryLine> *mem_lines = new std::vector<MemoryLine>;

    DisassemblyBuilder::build(dasm_lines, dasm->get_start(), dasm->get_end(), dasm->get_mapped_memory(), emu_ptr->labels->getLabels());
    MemoryBuilder::build(mem_lines, mem->get_start(), mem->get_end(), mem->get_mapped_memory());

    CpuStatus status = emu_ptr->get_status();

    SnapFile::Write(fileName, status, dasm_lines, mem_lines, settings);
}