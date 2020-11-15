#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "../common/common_defs.h"
#include <vector>
#include <QString>
#include <QTextStream>
#include <QFile>

struct Breakpoint
{
    offs_t address;
    bool is_enabled;
};

class BreakpointReader
{
public:
    static std::vector<Breakpoint> *Read(QString path, bool &success);
    static void Write(QString path, std::vector<Breakpoint> *breakpoints, bool &success);
};

#endif // BREAKPOINT_H