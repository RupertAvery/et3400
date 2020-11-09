#include "disassembly_builder.h"

std::vector<DisassemblyLine> *DisassemblyBuilder::build(offs_t start, offs_t end, uint8_t *memory, std::vector<Map> *maps)
{
    std::vector<DisassemblyLine> *lines = new std::vector<DisassemblyLine>;
    offs_t address = start;
    std::vector<Map>::iterator map = maps->begin();
    bool hasMaps = maps->size() > 0;
    //QChar filler = QLatin1Char('0');
    offs_t ptr = 0;
    while (address <= end)
    {
        if (hasMaps && address > map->start)
        {
            while (address > map->start)
            {
                map++;
            }
        }

        if (hasMaps && address == map->start)
        {
            if (map->type == DATA)
            {

                int i = 0;
                int line_count = 0;
                lines->push_back(DisassemblyLine{address, DisassemblyType::Comment, QString("; %1").arg(map->comment), NULL, NULL});
                while (address < map->end)
                {
                    offs_t save_address = address;
                    QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");
                    i = 0;
                    for (; address <= end && address <= map->end && i < 8; i++)
                    {
                        data = data.arg(memory[ptr], 2, 16, QChar('0')).toUpper();
                        address++;
                        ptr++;
                    }
                    for (; i < 8; i++)
                    {
                        data = data.arg(" ");
                    }
                    lines->push_back(DisassemblyLine{save_address, DisassemblyType::Data, data, NULL, NULL});
                    line_count++;
                }
                // if (line_count > 1)
                // {
                //     lines->push_back(DisassemblyLine{address - 1, DisassemblyType::Comment, QString("; END // %1").arg(map->comment), NULL, NULL});
                // }

                map++;
                if (map == maps->end())
                {
                    hasMaps = false;
                }
            }
        }
        else
        {
            DasmResult result = Disassembler::disassemble(&memory[ptr], address);
            QString opcodes = QString("%1 %2 %3");
            int i = 0;
            for (; i < result.byteLength; i++)
            {
                opcodes = opcodes.arg(memory[ptr + i], 2, 16, QChar('0')).toUpper();
            }
            for (; i < 3; i++)
            {
                opcodes = opcodes.arg("  ");
            }
            lines->push_back(DisassemblyLine{address, DisassemblyType::Assembly, opcodes, QString(result.instruction), result.operand});
            ptr += result.byteLength;
            address += result.byteLength;
        }
    }
    return lines;
}