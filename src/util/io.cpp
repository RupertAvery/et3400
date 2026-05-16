#include "io.h"

bool is_bin(QString filename)
{
    return filename.endsWith(".bin", Qt::CaseInsensitive);
}


void free_blocks(std::vector<data_block> *blocks)
{
    for (std::vector<data_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
    {
        free(it->data);
    }

    delete blocks;
}
