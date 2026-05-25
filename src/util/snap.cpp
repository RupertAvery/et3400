#include <QStringBuilder>
#include "snap.h"
#include "log.h"
#include "../common/util.h"

/*
 * Functions for writing the snapshot text file
 */

QString SnapFile::error;

bool SnapFile::Write(QString path, CpuStatus status, std::vector<DisassemblyLine> *dasm, std::vector<MemoryLine> *mem, SnapFileSettings settings)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        // QMessageBox::information(0, "error", file.errorString());
        return false;
    }

    QTextStream out(&file);

    out << "# Status Registers\r\n";
    out << "```\r\n";
    WriteStatus(out, status);
    out << "```";
    out << "\r\n";
    out << "\r\n";
    out << "# Disassembly View\r\n";
    out << "```\r\n";
    WriteDasm(out, dasm);
    out << "```";
    out << "\r\n";
    out << "\r\n";
    out << "# Memory View\r\n";
    out << "```\r\n";
    WriteMem(out, mem);
    out << "```";

    return true;
}

void SnapFile::WriteStatus(QTextStream &out, CpuStatus status)
{
    out << "PC:     "
        << toHex(status.pc)
        << "\r\nACCA:     "
        << toHex(status.acca, 2)
        << "\r\nACCB:     "
        << toHex(status.accb, 2)
        << "\r\nIX:     "
        << toHex(status.ix)
        << "\r\nSP:     "
        << toHex(status.sp)
        << "\r\nCC:     "
        << toBin(status.cc)
        << "\r\n          HINZVC";

    out << "\r\n";
}

void SnapFile::WriteDasm(QTextStream &out, std::vector<DisassemblyLine> *lines)
{
    std::vector<DisassemblyLine>::iterator line = lines->begin();

    while (line != lines->end())
    {
        bool is_comment = line->type == DisassemblyType::Comment;
        bool is_data = line->type == DisassemblyType::Data;

        if (is_comment)
        {

            out << line->opcodes;
        }
        else
        {
            out << toHex(line->address) % ":  ";

            if (is_data)
            {
                out << line->opcodes;
            }
            else
            {
                out << line->opcodes;
            }

            if (line->type == DisassemblyType::Assembly)
            {
                out << "   ";
                out << line->instruction << " ";
                out << line->operand;
            }
        }

        line++;
        out << "\r\n";
    }
}

void SnapFile::WriteMem(QTextStream &out, std::vector<MemoryLine> *lines)
{
    std::vector<MemoryLine>::iterator line = lines->begin();

    while (line != lines->end())
    {
        out << toHex(line->address) % ":  ";
        out << line->data;
        line++;
        out << "\r\n";
    }
}
