#include "memory_builder.h"
#include <qstringbuilder.h>

QChar filler = QLatin1Char('0');

void MemoryBuilder::build(std::vector<MemoryLine> *lines, offs_t start, offs_t end, uint8_t *memory)
{
    lines->clear();
    int ctr = 0;

    offs_t address = start;

    while (address <= end)
    {
        int i = 0;

        QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");

        while (address + i <= end && i < 8)
        {
            data = data.arg(memory[address + i - start], 2, 16, filler).toUpper();
            i++;
        }

        while (i < 8)
        {
            data = data.arg(0, 2, 16, filler).toUpper();
            i++;
        }

        lines->push_back(MemoryLine{address, data, i});

        address += i;
    }
}