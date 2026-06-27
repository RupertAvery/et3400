#ifndef BREAKPOINT_READER_H
#define BREAKPOINT_READER_H

#include "../common/common_defs.h"
#include <vector>
#include <QString>
#include <QTextStream>
#include <QFile>

struct Breakpoint
{
    offs_t address;
    int type; // 0 = execute, 1 = read, 2 = write
    QString description;
    QString condition;
    bool is_enabled;
    bool is_hidden;
};

class BreakpointReader
{
public:
    static std::vector<Breakpoint> *Read(QString path, bool &success);
    static void Write(QString path, std::vector<Breakpoint> *breakpoints, bool &success);
};

#endif // BREAKPOINT_READER_H