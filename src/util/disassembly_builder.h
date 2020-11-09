#ifndef DISASSEMBLY_BUILDER_H
#define DISASSEMBLY_BUILDER_H

#include <vector>
#include "map.h"
#include "../cpu/defs.h"
#include "../dasm/disassembler.h"
#include <QString>

enum DisassemblyType {
    Comment,
    Assembly,
    Data,
    Empty
};

struct DisassemblyLine
{
    offs_t address;
    DisassemblyType type;
    QString opcodes;
    QString instruction;
    QString operand;
    bool is_current;
    bool is_selected;
    bool has_breakpoint;
};

class DisassemblyBuilder
{
public:
    static std::vector<DisassemblyLine> *build(offs_t start, offs_t end, uint8_t *memory, std::vector<Map> *maps);
};

#endif // DISASSEMBLY_BUILDER_H