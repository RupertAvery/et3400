#include "breakpoint.h"
#include "csv.h"

void BreakpointReader::Write(QString path, std::vector<Breakpoint> *breakpoints, bool &success)
{
    std::vector<Breakpoint>::iterator current = breakpoints->begin();

    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        success = false;
        return;
    }

    QTextStream out(&file);

    while (current != breakpoints->end())
    {
        out << QString("%1").arg((*current).address, 4, 16, QChar('0')).toUpper() << ","
            << ((*current).is_enabled ? QString("Yes") : QString("No"))
            << "\r\n";

        current++;
    }

    out.flush();
    file.close();
    success = true;
}
std::vector<Breakpoint> *BreakpointReader::Read(QString path, bool &success)
{
    std::vector<Breakpoint> *breakpoints = new std::vector<Breakpoint>;
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        success = false;
        return breakpoints;
    }

    QTextStream in(&file);
    QStringList parts;

    while (readCSVRow(in, &parts))
    {
        bool bStatus = false;
        uint32_t start = parts.at(0).toUInt(&bStatus, 16);
        bool is_enabled = parts.at(1).toLower() == "yes";
        breakpoints->push_back(Breakpoint{start, is_enabled});
    }

    success = true;
    return breakpoints;
}