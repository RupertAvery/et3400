#ifndef SNAP_H
#define SNAP_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <vector>
#include "../common/common_defs.h"
#include "../util/disassembly_builder.h"
#include "../util/memory_builder.h"

struct SnapFileSettings
{
    bool include_status; 
    bool include_dasm; 
    bool include_mem; 
};

class SnapFile
{
public:
    static bool Write(QString path, CpuStatus status, std::vector<DisassemblyLine> *dasm, std::vector<MemoryLine> *mem, SnapFileSettings settings);
    static QString error;

private:
    static void WriteStatus(QTextStream &out, CpuStatus status);
    static void WriteDasm(QTextStream &out, std::vector<DisassemblyLine> *dasm);
    static void WriteMem(QTextStream &out, std::vector<MemoryLine> *mem);
};

#endif // SNAP_H
