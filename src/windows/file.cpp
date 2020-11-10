#include "file.h"

void File::load_ram(QWidget *parent, et3400emu *emu_ptr)
{
    QString fileName = QFileDialog::getOpenFileName(parent,
                                                    "Open .S19 File", "", "Assembly Files (*.obj *.s19)");

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