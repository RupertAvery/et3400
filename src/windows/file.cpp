#include "file.h"

void File::load_ram(QWidget *parent, et3400emu *emu_ptr)
{
    QString fileName = QFileDialog::getOpenFileName(parent,
                                                    "Load File to RAM", "", "SREC Files (*.obj *.s19)");

    std::vector<srec_block> *blocks = new std::vector<srec_block>;

    // pause emulation to avoid overwriting memory while executing
    emu_ptr->stop();

    // load S19 blocks
    if (SrecReader::Read(fileName, blocks))
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

    // reset and resume emulation
    emu_ptr->reset();
    emu_ptr->start();
}

void File::save_ram(QWidget *parent, et3400emu *emu_ptr)
{
    QString fileName = QFileDialog::getSaveFileName(parent,
                                                    "Save RAM Contents", "", "SREC Files (*.s19)");

    std::vector<srec_block> *blocks = new std::vector<srec_block>;

    // pause emulation to avoid reading changing memory while executing
    emu_ptr->stop();

    uint8_t *memory = emu_ptr->memory_map->get_block_device(0x0000)->get_mapped_memory();

    int bytecount = 0;
    int i = 0;
    int address = 0;

    while (address < 0x400)
    {
        bytecount = 0;
        int save_address = address;
        while (memory[address] != 0 && bytecount < 16)
        {
            address++;
            bytecount++;
        }
        i = 0;
        while (memory[address + i] == 0 && bytecount + i < 16)
        {
            i++;
        }
        if (i < 16 - bytecount)
        {
            bytecount += i;
            address += i;
        }
        blocks->push_back(srec_block{bytecount, save_address, &memory[save_address]});
        i = 0;
        while (memory[address + i] == 0)
        {
            i++;
        }
        address += i;
    }

    // load S19 blocks
    SrecReader::Write(fileName, blocks);

    delete blocks;

    // reset and resume emulation
    emu_ptr->start();
}