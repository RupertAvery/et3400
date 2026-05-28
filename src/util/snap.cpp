#include <QStringBuilder>
#include "snap.h"
#include "log.h"
#include "../common/util.h"
#include "../common/newline.h"

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

    if (settings.include_status)
    {
        out << "Status Registers" << NEWLINE;
        out << "================" << NEWLINE;
        WriteStatus(out, status);
        out << NEWLINE;
        out << NEWLINE;
    }

    if (settings.include_dasm)
    {
        out << "Disassembly View" << NEWLINE;
        out << "================" << NEWLINE;
        WriteDasm(out, dasm);
        out << NEWLINE;
        out << NEWLINE;
    }

    if (settings.include_mem)
    {
        out << NEWLINE;
        out << NEWLINE;
        out << "# Memory View" << NEWLINE;
        out << "=============" << NEWLINE;
        WriteMem(out, mem);
        out << NEWLINE;
        out << NEWLINE;
    }

    return true;
}

void SnapFile::WriteStatus(QTextStream &out, CpuStatus status)
{
    out << "PC:        "
        << toHex(status.pc) << NEWLINE
        << "ACCA:        "
        << toHex(status.acca, 2) << NEWLINE
        << "ACCB:        "
        << toHex(status.accb, 2) << NEWLINE
        << "IX:        "
        << toHex(status.ix) << NEWLINE
        << "SP:        "
        << toHex(status.sp) << NEWLINE
        << "CC:     "
        << toBin(status.cc) << NEWLINE
        << "        --HINZVC";

    out << NEWLINE;
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
        out << NEWLINE;
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
        out << NEWLINE;
    }
}
