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
    Map* map;
    int bytes;
};

class DisassemblyBuilder
{
public:
    static void build(std::vector<DisassemblyLine>* lines, offs_t start, offs_t end, uint8_t *memory, std::vector<Map> *maps);
private:
    static void disassemble(std::vector<DisassemblyLine>* lines, uint8_t* memory, int& ptr, offs_t &address, int& id, int parentId, Map* map);
};

#endif // DISASSEMBLY_BUILDER_H